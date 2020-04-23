#include "Editor.h"

#include "Cmpt/Roamer.h"
#include "Cmpt/Hierarchy.h"
#include "Cmpt/Inspector.h"

#include <UScene/core.h>

using namespace Ubpa;
using namespace std;

Editor::Editor()
	: scene{ new Scene{ "Editor Scene" } }
{
	scene->CreateSObj<Cmpt::Camera, Cmpt::Roamer>("roamer");
	scene->CreateSObj<Cmpt::Hierarchy, Cmpt::Inspector>("hierarchy_inspector");
}

Editor::~Editor() {
	delete scene;
}

void Editor::SetCamera(SObj* cameraobj) {
	auto l2w = cameraobj->Get<Cmpt::L2W>();
	auto src_cam = cameraobj->Get<Cmpt::Camera>();
	auto dst_cam = cameraobj->Get<Cmpt::Camera>();
	auto roamerobj = scene->root->GetSObjInTreeWith<Cmpt::Roamer>();

	roamerobj->Get<Cmpt::Position>()->value = l2w->WorldPos();
	roamerobj->Get<Cmpt::Rotation>()->value = l2w->WorldRot();

	*dst_cam = *src_cam;
}

void Editor::OnRegist() {
	CmptRegister::Instance().Regist<Cmpt::Roamer>();
	CmptRegister::Instance().Regist<Cmpt::Hierarchy>();
	CmptRegister::Instance().Regist<Cmpt::Inspector>();
}
