#include <Engine/MeshEdit/MinSurf.h>

#include <Engine/Primitive/TriMesh.h>

#include <Eigen/Sparse>

using namespace Ubpa;

using namespace std;
using namespace Eigen;

MinSurf::MinSurf(Ptr<TriMesh> triMesh)
	: heMesh(make_shared<HEMesh<V>>())
{
	Init(triMesh);
}

void MinSurf::Clear() {
	heMesh->Clear();
	triMesh = nullptr;
}

bool MinSurf::Init(Ptr<TriMesh> triMesh) {
	Clear();

	if (triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::MinSurf::Init:\n"
			"\t""trimesh is invalid\n");
		return false;
	}

	// init half-edge structure
	size_t nV = triMesh->GetPositions().size();
	vector<vector<size_t>> triangles;
	triangles.reserve(triMesh->GetTriangles().size());
	for (auto triangle : triMesh->GetTriangles())
		triangles.push_back({ triangle->idx[0], triangle->idx[1], triangle->idx[2] });
	heMesh->Reserve(nV);
	heMesh->Init(triangles);

	if (!heMesh->IsTriMesh() || !heMesh->HaveBoundary()) {
		printf("ERROR::MinSurf::Init:\n"
			"\t""trimesh is not a triangle mesh or hasn't a boundaries\n");
		heMesh->Clear();
		return false;
	}

	// triangle mesh's positions ->  half-edge structure's positions
	for (int i = 0; i < nV; i++) {
		auto v = heMesh->Vertices().at(i);
		v->pos = triMesh->GetPositions()[i].cast_to<vecf3>();
	}

	this->triMesh = triMesh;
	return true;
}

bool MinSurf::Run() {
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::MinSurf::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	Minimize();

	// half-edge structure -> triangle mesh
	size_t nV = heMesh->NumVertices();
	size_t nF = heMesh->NumPolygons();
	vector<pointf3> positions;
	vector<unsigned> indice;
	positions.reserve(nV);
	indice.reserve(3 * nF);
	for (auto v : heMesh->Vertices())
		positions.push_back(v->pos.cast_to<pointf3>());
	for (auto f : heMesh->Polygons()) { // f is triangle
		for (auto v : f->BoundaryVertice()) // vertices of the triangle
			indice.push_back(static_cast<unsigned>(heMesh->Index(v)));
	}

	triMesh->Init(indice, positions);

	return true;
}

void MinSurf::Minimize() {
	
	Laplace_init();

	Predecomposition();

	Solve();


	cout << "INFO::MinSurf::Minimize:" << endl
		<< "\t" << "Success" << endl;
}

void MinSurf::Laplace_init()
{
	size_t nV = heMesh->NumVertices();

	//	generate Laplace Matrix
	vector<Eigen::Triplet<double>> Lij;

	for (size_t i = 0; i < nV; i++)
	{
		V* v1 = heMesh->Vertices()[i];
		Lij.push_back(Eigen::Triplet<double>(i, i, 1));
		if(!v1->IsBoundary())
		{
			double connect_num = v1->AdjVertices().size();
			for (size_t j = 0; j < connect_num; j++)
			{
				Lij.push_back(Eigen::Triplet<double>(i, heMesh->Index(v1->AdjVertices()[j]), -1 / connect_num));
			}
		}
	}

	Laplace_matrix.resize(nV, nV);
	Laplace_matrix.setZero();

	Laplace_matrix.setFromTriplets(Lij.begin(), Lij.end());
	//Laplace_matrix.makeCompressed();
}

void MinSurf::Predecomposition()
{
	solver.compute(Laplace_matrix);
	if (solver.info() != Eigen::Success)
	{
		throw std::exception("Compute Matrix Is Error!");
		return;
	}
}

void MinSurf::Solve()
{
	size_t nV = heMesh->NumVertices();
	Eigen::VectorXd x(nV), y(nV), z(nV);
	Eigen::VectorXd bx(nV), by(nV), bz(nV);
	bx.setZero(); by.setZero(); bz.setZero();

	for (size_t i = 0; i < nV; i++)
	{
		V* v = heMesh->Vertices()[i];
		if (v->IsBoundary())
		{
			bx(i) = v->pos.at(0);
			by(i) = v->pos.at(1);
			bz(i) = v->pos.at(2);
		}
	}

	x = solver.solve(bx);
	y = solver.solve(by);
	z = solver.solve(bz);

	for (size_t i = 0; i < nV; i++)
	{
		heMesh->Vertices()[i]->pos.at(0) = x(i);
		heMesh->Vertices()[i]->pos.at(1) = y(i);
		heMesh->Vertices()[i]->pos.at(2) = z(i);
	}
}