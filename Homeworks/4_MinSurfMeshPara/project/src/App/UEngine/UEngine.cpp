#include "UEngine.h"

#include "GenScene.h"

#include <UI/Hierarchy.h>
#include <UI/Attribute.h>
#include <UI/Setting.h>

#include <Qt/PaintImgOpCreator.h>
#include <Qt/OpThread.h>

#include <Engine/Viewer/RTX_Renderer.h>
#include <Engine/Viewer/PathTracer.h>
#include <Engine/Viewer/Viewer.h>
#include <Engine/Scene/Scene.h>
#include <Engine/Scene/SObj.h>
#include <Engine/Scene/CmptGeometry.h>
#include <Engine/Scene/CmptSimu/CmptSimulate.h>
#include <Engine/Scene/CmptSimu/MassSpring.h>
#include <Engine/Primitive/TriMesh.h>
#include <Basic/Image.h>
#include <Basic/Op/LambdaOp.h>
#include <Basic/GStorage.h>
#include <Basic/Math.h>

#include <ROOT_PATH.h>

#include <qdebug.h>
#include <qtimer.h>
#include <qfiledialog.h>
#include <qevent.h>

#include <synchapi.h>

using namespace Ubpa;

using namespace std;
using namespace Ui;

UEngine::UEngine(QWidget *parent)
	: QMainWindow(parent), maxDepth(5), maxLoop(20)
{
	ui.setupUi(this);

	// hide titlebar of dock_Top
	QWidget* lTitleBar = ui.dock_Top->titleBarWidget();
	QWidget* lEmptyWidget = new QWidget();
	ui.dock_Top->setTitleBarWidget(lEmptyWidget);
	delete lTitleBar;

	// update per frame
	QTimer * timer = new QTimer;
	timer->callOnTimeout([this]() {
		ui.OGLW_Raster->update();
		ui.OGLW_RayTracer->update();
	});

	const size_t fps = 60;
	timer->start(1000 / fps);

	scene = GenScene(11);

	// viewer
	viewer = Viewer::New(ui.OGLW_Raster, scene, Viewer::RasterType::DeferredPipeline);

	// raytracer
	PaintImgOpCreator pioc(ui.OGLW_RayTracer);
	paintImgOp = pioc.GenScenePaintOp();

	auto generator = [&]()->Ptr<PathTracer>{
		auto pathTracer = PathTracer::New();
		pathTracer->maxDepth = maxDepth;

		return pathTracer;
	};

	rtxRenderer = RTX_Renderer::New(generator);
	rtxRenderer->maxLoop = maxLoop;
	
	// init ui

	Hierarchy::GetInstance()->Init(scene, ui.tree_Hierarchy);

	Attribute::GetInstance()->Init(ui.tbox_Attribute, ui.OGLW_Raster);

	InitSetting();


	QTimer* timer_simu = new QTimer(this);
	timer_simu->start(30);
	timer_simu->stop();
	connect(ui.btn_SimulateStart, SIGNAL(clicked()), timer_simu, SLOT(start()));
	connect(ui.btn_SimulateStop, SIGNAL(clicked()), timer_simu, SLOT(stop()));
	connect(timer_simu, SIGNAL(timeout()), this, SLOT(updateSimulate()));

}

void UEngine::on_btn_RenderStart_clicked(){
	ui.btn_RenderStart->setEnabled(false);
	ui.btn_RenderStop->setEnabled(true);
	ui.btn_SaveRayTracerImg->setEnabled(true);

	ui.tree_Hierarchy->setEnabled(false);
	ui.frame_Setting->setEnabled(false);
	ui.tbox_Attribute->setEnabled(false);

	auto drawImgThread = OpThread::New();
	drawImgThread->UIConnect(this, &UEngine::UI_Op);

	auto drawImgOp = LambdaOp_New([=]() {
		paintImgOp->SetOp(1024, 768);
		auto img = paintImgOp->GetImg();

		auto controller = OpThread::New();
		controller->UIConnect(this, &UEngine::UI_Op);
		auto controllOp = LambdaOp_New([=]() {
			while (!drawImgThread->IsStop()) {
				controller->UI_Op_Run(LambdaOp_New([&, this]() {
					ui.rtxProgress->setValue(rtxRenderer->ProgressRate() * ui.rtxProgress->maximum());
				}));
				Sleep(100);
			}
			rtxRenderer->Stop();
		});
		controller->SetOp(controllOp);
		controller->start();

		rtxRenderer->Run(scene, img);

		controller->terminate();
		drawImgThread->UI_Op_Run(LambdaOp_New([=]() {
			ui.btn_RenderStart->setEnabled(true);
			ui.btn_RenderStop->setEnabled(false);

			ui.tree_Hierarchy->setEnabled(true);
			ui.frame_Setting->setEnabled(true);
			ui.tbox_Attribute->setEnabled(true);
			//ui.rtxProgress->setValue(rtxRenderer->ProgressRate() * ui.rtxProgress->maximum());
		}));
	});
	drawImgThread->SetOp(drawImgOp);
	GS::Reg("drawImgThread", drawImgThread);

	drawImgThread->start();
}

void UEngine::on_btn_RenderStop_clicked() {
	ui.btn_RenderStop->setEnabled(false);
	Ptr<OpThread> drawImgThread;
	GS::GetV("drawImgThread", drawImgThread);
	if (!drawImgThread)
		return;
	drawImgThread->Stop();
}

void UEngine::on_btn_SaveRayTracerImg_clicked() {
	QString fileName = QFileDialog::getSaveFileName(this,
		tr("����ͼƬ"),
		"./",
		tr("Image Files (*.png)"));
	if (fileName.isEmpty())
		return;
	
	// should not use grabFramebuffer to get the image because of the border
	auto img = Image::New(*paintImgOp->GetImg());
	img->SaveAsPNG(fileName.toStdString(), true);
}

void UEngine::updateSimulate()
{
	auto cmpts = Hierarchy::GetInstance()->GetRoot()->GetComponentsInChildren<CmptSimulate>();
	for (auto cmpt : cmpts) {
		auto mesh = CastTo<MassSpring>(cmpt->GetMesh());
		if (mesh) {
			//cout << "ooll1" << endl;
			mesh->RunSimu();
			ui.OGLW_Raster->DirtyVAO(mesh->GetTriMesh());
			
		}
	}
}

void UEngine::on_btn_SaveRasterImg_clicked() {
	QString fileName = QFileDialog::getSaveFileName(this,
		tr("����ͼƬ"),
		"./",
		tr("Image Files (*.png)"));
	if (fileName.isEmpty())
		return;

	ui.OGLW_Raster->grabFramebuffer().save(fileName);
}

void UEngine::UI_Op(Ptr<Op> op) {
	op->Run();
}

void UEngine::InitSetting() {
	auto setting = Setting::GetInstance();
	setting->Init(ui.frame_Setting);

	setting->AddTitle("[ RTX_Renderer ]");
	setting->AddEditVal("- Sample Num", maxLoop, 1, 1024, [&](int val) {
		rtxRenderer->maxLoop = val;
	});
	
	setting->AddTitle("[ PathTracer ]");
	setting->AddEditVal("- Max Depth", maxDepth, 1, 100, [&](int val) {
		maxDepth = val;
	});

	setting->AddTitle("[ Viewer ]");
	Grid::pSlotMap slotmap = std::make_shared<Grid::SlotMap>();
	(*slotmap)["Deferred"] = [this]() {viewer->SetRaster(Viewer::RasterType::DeferredPipeline); };
	(*slotmap)["Wireframe"] = [this]() {viewer->SetRaster(Viewer::RasterType::WireframeRaster); };
	setting->AddComboBox("Raster Type", "Deferred", slotmap);
}
