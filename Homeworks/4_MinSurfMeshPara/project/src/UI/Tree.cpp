#include <UI/Tree.h>

#include <UI/Hierarchy.h>
#include <UI/Attribute.h>

#include <Basic/GStorage.h>
#include <Engine/Scene/SObj.h>
#include <Engine/Primitive/TriMesh.h>
#include <Engine/Scene/AllComponents.h>
#include <Engine/Material/AllBSDFs.h>
#include <Engine/Material/Material.h>
#include <Engine/Primitive/Sphere.h>
#include <Engine/Primitive/Plane.h>
#include <Engine/Primitive/Disk.h>
#include <Engine/Primitive/Capsule.h>

#include <Engine/Light/AreaLight.h>
#include <Engine/Light/PointLight.h>
#include <Engine/Light/SpotLight.h>
#include <Engine/Light/DirectionalLight.h>
#include <Engine/Light/InfiniteAreaLight.h>
#include <Engine/Light/SphereLight.h>
#include <Engine/Light/DiskLight.h>
#include <Engine/Light/CapsuleLight.h>

#include <qmenu.h>
#include <qdrag.h>
#include <qevent.h>
#include <qmimedata.h>
#include <qfiledialog.h>

using namespace Ubpa;

using namespace std;

Tree::Tree(QWidget *parent)
	: QTreeWidget(parent), needDragItem(nullptr), canDrag(false) {

	setAcceptDrops(true);

	connect(this, &QTreeWidget::currentItemChanged, this, &Tree::on_tree_Hierarchy_currentItemChanged);
	connect(this, &QTreeWidget::itemClicked, this, &Tree::on_tree_Hierarchy_itemClicked);
	connect(this, &QTreeWidget::itemPressed, this, &Tree::on_tree_Hierarchy_itemPressed);

}

void Tree::dragEnterEvent(QDragEnterEvent *event) {
	//printf("drag enter\n");
	event->setDropAction(Qt::MoveAction);
	event->accept();
	//QTreeWidget::dragEnterEvent(event);
}

void Tree::dragLeaveEvent(QDragLeaveEvent *event) {
	//printf("drag leave\n");
	event->accept();
	//QTreeWidget::dragLeaveEvent(event);
}

void Tree::dragMoveEvent(QDragMoveEvent *event) {
	//printf("drag move\n");
	event->setDropAction(Qt::MoveAction);
	event->accept();
	auto mE = new QMouseEvent(QEvent::MouseMove, event->posF(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	mouseMoveEvent(mE);
	delete mE;
	//QTreeWidget::dragMoveEvent(event);
}

void Tree::dropEvent(QDropEvent *event) {
	//printf("drop\n");
	event->setDropAction(Qt::MoveAction);
	event->accept();
	
	string key = event->mimeData()->text().toStdString();
	//printf("text: %s\n", key.c_str());

	QTreeWidgetItem * item;
	GS::GetV(key, item);
	GS::UnReg<QTreeWidgetItem *>(key);

	Hierarchy::GetInstance()->Move(item, currentItem());
	//QTreeWidget::dropEvent(event);
}


void Tree::on_tree_Hierarchy_itemClicked(QTreeWidgetItem *item, int column) {
	auto sobj = Hierarchy::GetInstance()->GetSObj(item);
	Attribute::GetInstance()->SetSObj(sobj);
}

void Tree::on_tree_Hierarchy_itemPressed(QTreeWidgetItem *item, int column) {
	if (canDrag)
		needDragItem = item;
}

void Tree::on_tree_Hierarchy_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
	if (needDragItem && current != previous) {
		auto drag = new QDrag(this);

		auto mimedata = new QMimeData;
		GS::Reg("dragItem", needDragItem);
		needDragItem = nullptr;
		mimedata->setText("dragItem");
		drag->setMimeData(mimedata);

		drag->exec();
	}
}

void Tree::on_tree_Hierarchy_itemChanged(QTreeWidgetItem *item, int column) {
}

void Tree::on_tree_Hierarchy_itemEntered(QTreeWidgetItem *item, int column) {
}

void Tree::on_tree_Hierarchy_itemActivated(QTreeWidgetItem *item, int column) {
}

void Tree::on_tree_Hierarchy_itemDoubleClicked(QTreeWidgetItem *item, int column) {
}

void Tree::on_tree_Hierarchy_itemSelectionChanged() {
}

void Tree::mousePressEvent(QMouseEvent *event){
	canDrag = event->button() == Qt::LeftButton;
	needDragItem = nullptr;
	QTreeWidget::mousePressEvent(event);
}

void Tree::mouseReleaseEvent(QMouseEvent *event){
	canDrag = false;
	needDragItem = nullptr;
	QTreeWidget::mouseReleaseEvent(event);
}

void Tree::contextMenuEvent(QContextMenuEvent *event) {
	QMenu mainMenu;

	if (currentItem()) {
		mainMenu.addAction("Rename", this, []() {
			Hierarchy::GetInstance()->RenameCurItem();
		});
	}

	if (currentItem() && Hierarchy::GetInstance()->GetSObj(currentItem()) != Hierarchy::GetInstance()->GetRoot()) {
		mainMenu.addAction("Delete", this, []() {
			Hierarchy::GetInstance()->DeleteSObj();
		});
	}

	auto spitLine0 = new QAction;
	spitLine0->setSeparator(true);
	mainMenu.addAction(spitLine0);

	mainMenu.addAction("Create Empty", this, []() {
		auto sobj = Hierarchy::GetInstance()->CreateSObj("SObj");
		sobj->AddComponent<CmptTransform>();
	});

	// gen obj
	auto genObjMenu = new QMenu;
	genObjMenu->setTitle("Create 3D Object");

	genObjMenu->addAction("Cube", this, []() {
		auto sobj = Hierarchy::GetInstance()->CreateSObj("Cube");
		sobj->AddComponent<CmptTransform>();
		sobj->AddComponent<CmptGeometry>(TriMesh::GenCube());
		sobj->AddComponent<CmptMaterial>(BSDF_Frostbite::New());
	});

	genObjMenu->addAction("Sphere", this, []() {
		auto sobj = Hierarchy::GetInstance()->CreateSObj("Sphere");
		sobj->AddComponent<CmptTransform>();
		sobj->AddComponent<CmptGeometry>(Sphere::New());
		sobj->AddComponent<CmptMaterial>(BSDF_Frostbite::New());
	});

	genObjMenu->addAction("Plane", this, []() {
		auto sobj = Hierarchy::GetInstance()->CreateSObj("Plane");
		sobj->AddComponent<CmptTransform>();
		sobj->AddComponent<CmptGeometry>(Plane::New());
		sobj->AddComponent<CmptMaterial>(BSDF_Frostbite::New());
	});

	genObjMenu->addAction("Disk", this, []() {
		auto sobj = Hierarchy::GetInstance()->CreateSObj("Disk");
		sobj->AddComponent<CmptTransform>();
		sobj->AddComponent<CmptGeometry>(Disk::New());
		sobj->AddComponent<CmptMaterial>(BSDF_Frostbite::New());
	});

	genObjMenu->addAction("Capsule", this, []() {
		auto sobj = Hierarchy::GetInstance()->CreateSObj("Capsule");
		sobj->AddComponent<CmptTransform>();
		sobj->AddComponent<CmptGeometry>(Capsule::New());
		sobj->AddComponent<CmptMaterial>(BSDF_Frostbite::New());
	});

	mainMenu.addMenu(genObjMenu);

	// light
	auto genLightMenu = new QMenu;
	genLightMenu->setTitle("Create Light");

	genLightMenu->addAction("Area Light", this, []() {
		auto sobj = Hierarchy::GetInstance()->CreateSObj("Area Light");
		sobj->AddComponent<CmptTransform>();
		sobj->AddComponent<CmptLight>(AreaLight::New());
		sobj->AddComponent<CmptGeometry>(Plane::New());
		sobj->AddComponent<CmptMaterial>(BSDF_Emission::New());
	});

	genLightMenu->addAction("Point Light", this, []() {
		auto sobj = Hierarchy::GetInstance()->CreateSObj("Point Light");
		sobj->AddComponent<CmptTransform>();
		sobj->AddComponent<CmptLight>(PointLight::New());
	});

	genLightMenu->addAction("Directional Light", this, []() {
		auto sobj = Hierarchy::GetInstance()->CreateSObj("Directional Light");
		sobj->AddComponent<CmptTransform>();
		sobj->AddComponent<CmptLight>(DirectionalLight::New());
	});

	genLightMenu->addAction("Spot Light", this, []() {
		auto sobj = Hierarchy::GetInstance()->CreateSObj("Spot Light");
		sobj->AddComponent<CmptTransform>();
		sobj->AddComponent<CmptLight>(SpotLight::New());
	});

	genLightMenu->addAction("Infinite Area Light", this, []() {
		auto sobj = Hierarchy::GetInstance()->CreateSObj("Infinite Area Light");
		sobj->AddComponent<CmptTransform>();
		sobj->AddComponent<CmptLight>(InfiniteAreaLight::New(nullptr));
	});

	genLightMenu->addAction("Sphere Light", this, []() {
		auto sobj = Hierarchy::GetInstance()->CreateSObj("Sphere Light");
		sobj->AddComponent<CmptTransform>();
		sobj->AddComponent<CmptLight>(SphereLight::New());
		sobj->AddComponent<CmptGeometry>(Sphere::New());
		sobj->AddComponent<CmptMaterial>(BSDF_Emission::New());
	});

	genLightMenu->addAction("Disk Light", this, []() {
		auto sobj = Hierarchy::GetInstance()->CreateSObj("Disk Light");
		sobj->AddComponent<CmptTransform>();
		sobj->AddComponent<CmptLight>(DiskLight::New());
		sobj->AddComponent<CmptGeometry>(Disk::New());
		sobj->AddComponent<CmptMaterial>(BSDF_Emission::New());
	});

	genLightMenu->addAction("Capsule Light", this, []() {
		auto sobj = Hierarchy::GetInstance()->CreateSObj("Capsule Light");
		sobj->AddComponent<CmptTransform>();
		sobj->AddComponent<CmptLight>(CapsuleLight::New());
		sobj->AddComponent<CmptGeometry>(Capsule::New());
		sobj->AddComponent<CmptMaterial>(BSDF_Emission::New());
	});

	mainMenu.addMenu(genLightMenu);

	mainMenu.addAction("Create Camera", this, []() {
		auto sobj = Hierarchy::GetInstance()->CreateSObj("Camera");
		sobj->AddComponent<CmptTransform>();
		sobj->AddComponent<CmptCamera>();
	});

	auto spitLine1 = new QAction;
	spitLine1->setSeparator(true);
	mainMenu.addAction(spitLine1);

	// Load Save
	mainMenu.addAction("Load SObj", this, [=]() {
		QString fileName = QFileDialog::getOpenFileName(this,
			tr("Load SObj"),
			"./",
			tr("SObj Files (*.sobj *.obj *.FBX *.tet)"));

		if (fileName.isEmpty())
			return;

		auto sobj = SObj::Load(fileName.toStdString());
		if (sobj == nullptr)
			return;

		Hierarchy::GetInstance()->BindSObj(sobj);
	});

	if (currentItem()) {
		mainMenu.addAction("Save SObj", this, [=]() {
			QString fileName = QFileDialog::getSaveFileName(this,
				tr("Save SObj"),
				"./",
				tr("SObj Files (*.sobj)"));

			if (fileName.isEmpty())
				return;

			auto sobj = Hierarchy::GetInstance()->GetSObj(currentItem());
			if (sobj == nullptr)
				return;

			sobj->Save(fileName.toStdString());
		});
	}

	mainMenu.exec(QCursor::pos());
}
