#include <Engine/MeshEdit/Paramaterize.h>

#include <Engine/MeshEdit/MinSurf.h>

#include <Engine/Primitive/TriMesh.h>

using namespace Ubpa;

using namespace std;

Paramaterize::Paramaterize(Ptr<TriMesh> triMesh) 
	: heMesh(make_shared<HEMesh<V>>())
{
	Init(triMesh);
}

void Paramaterize::Clear() {
	heMesh->Clear();
	triMesh = nullptr;
}

bool Paramaterize::Init(Ptr<TriMesh> triMesh) {
	Clear();

	if (triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::Paramaterize::Init:\n"
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
		printf("ERROR::Paramaterize::Init:\n"
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

bool Paramaterize::Run() {
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::MinSurf::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	Paramaterization();

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
	triMesh->Update(texcoords);
	return true;
}


void Paramaterize::Paramaterization()
{
	Init_Boundary();
	Set_Boundary();
	Init_Matrix();
	Predecomposition();
	Solve();
}

double Paramaterize::Distance(V* v1, V* v2)
{
	return sqrt((v1->pos.at(0) - v2->pos.at(0)) * (v1->pos.at(0) - v2->pos.at(0)) + (v1->pos.at(1) - v2->pos.at(1)) * (v1->pos.at(1) - v2->pos.at(1)) +
		(v1->pos.at(2) - v2->pos.at(2)) * (v1->pos.at(2) - v2->pos.at(2)));
}

double Paramaterize::Cosine(V* v0, V* v1, V* v2)
{
	double edge1 = Distance(v0, v1);
	double edge2 = Distance(v1, v2);
	double edge3 = Distance(v0, v2);
	return (edge1 * edge1 + edge2 * edge2 - edge3 * edge3) / (2 * edge1 * edge2);
}

double Paramaterize::Cotangent(V* v0, V* v1, V* v2)
{
	double cosine = Cosine(v0, v1, v2);
	return cosine / sqrt(1 - cosine * cosine);
}

void Paramaterize::Set_Display()
{
	display_status = kon;
}

void Paramaterize::Init_Boundary()
{
	size_t nV = heMesh->NumVertices();

	std::vector<size_t> index_temp;
	for (size_t i = 0; i < nV; i++)
	{
		V* v = heMesh->Vertices()[i];
		if (v->IsBoundary())
		{
			index_temp.push_back(i);
		}
	}
	size_t nB = index_temp.size();
	size_t flag = 0;
	while (index_temp.size() > 0)
	{
		if (boundary_index.size() == 0)
		{
			boundary_index.push_back(index_temp[0]);
			index_temp.erase(index_temp.begin());
		}
		for (size_t i = 0; i < index_temp.size(); i++)
		{
			if (heMesh->Vertices()[boundary_index[flag]]->IsConnectedWith(heMesh->Vertices()[index_temp[i]]))
			{
				boundary_index.push_back(index_temp[i]);
				index_temp.erase(index_temp.begin() + i);
				flag++;
				break;
			}
		}
		if (flag > 1 && heMesh->Vertices()[boundary_index[0]]->IsConnectedWith(heMesh->Vertices()[boundary_index.back()]))
		{
			return;
		}
	}
}

void Paramaterize::Set_Boundary_Square()
{
	boundary_type = kSquare;
}

void Paramaterize::Set_Boundary_Circle()
{
	boundary_type = kCircle;
}

void Paramaterize::Set_Boundary()
{
	if (boundary_type == kSquare)
	{
		Boundary_Setting = make_shared<Boundary_Square>();
	}
	else if (boundary_type == kCircle)
	{
		Boundary_Setting = make_shared<Boundary_Circle>();
	}
	Boundary_Setting->Set_Boundary(boundary_index, boundary_list);
}

void Paramaterize::Set_Uniform_Method()
{
	barycentric_type = kUniform;
}

void Paramaterize::Set_Cotangent_Method()
{
	barycentric_type = kCotangent;
}

void Paramaterize::Init_Matrix_Uniform()
{

	size_t nV = heMesh->NumVertices();

	//	generate Laplace Matrix
	vector<Eigen::Triplet<double>> Lij;

	for (size_t i = 0; i < nV; i++)
	{
		V* v1 = heMesh->Vertices()[i];
		Lij.push_back(Eigen::Triplet<double>(i, i, 1));
		if (!v1->IsBoundary())
		{
			double connect_num = 0;
			vector<size_t> index_list;
			for (size_t j = 0; j < nV; j++)
			{
				V* v2 = heMesh->Vertices()[j];
				if (v1->IsConnectedWith(v2))
				{
					index_list.push_back(j);
					connect_num++;
				}
			}
			for (size_t j = 0; j < index_list.size(); j++)
			{
				Lij.push_back(Eigen::Triplet<double>(i, index_list[j], -1 / connect_num));
			}
		}
	}

	Laplace_matrix.resize(nV, nV);
	Laplace_matrix.setZero();

	Laplace_matrix.setFromTriplets(Lij.begin(), Lij.end());

}

void Paramaterize::Init_Matrix_Cotangent()
{
	size_t nV = heMesh->NumVertices();
	vector<Eigen::Triplet<double>> Lij;
	
	for (size_t i = 0; i < nV; i++)
	{
		V* v = heMesh->Vertices()[i];
		Lij.push_back(Eigen::Triplet<double>(i, i, 1));

		if (!v->IsBoundary())
		{
			int last = 0, next = 0;
			vector<size_t> neighbor_index;
			vector<double> weight_list;
			double weight_sum=0;
			for (int j = 0; j < v->AdjVertices().size(); j++)
			{
				if (j == 0)
				{
					last = v->AdjVertices().size() - 1;
				}
				else
				{
					last = j - 1;
				}
				if (j == v->AdjVertices().size() - 1)
				{
					next = 0;
				}
				else
				{
					next = j + 1;
				}
				V* v1 = v->AdjVertices()[last];
				V* v2 = v->AdjVertices()[j];
				V* v3 = v->AdjVertices()[next];

				weight_list.push_back(Cotangent(v, v1, v2) + Cotangent(v, v3, v2));
				neighbor_index.push_back(heMesh->Index(v2));
				weight_sum += weight_list.back();
			}
			for (size_t j = 0; j < neighbor_index.size(); j++)
			{
				Lij.push_back(Eigen::Triplet<double>(i, neighbor_index[j], -weight_list[j]/weight_sum));
				cout << weight_list[j] / weight_sum << " ";
			}
			cout << endl;
		}		
	}
	Laplace_matrix.resize(nV, nV);
	Laplace_matrix.setZero();

	Laplace_matrix.setFromTriplets(Lij.begin(), Lij.end());
}

void Paramaterize::Init_Matrix()
{
	if (barycentric_type == kCotangent)
		Init_Matrix_Cotangent();
	else if (barycentric_type == kUniform)
		Init_Matrix_Uniform();
}

void Paramaterize::Predecomposition()
{
	solver.compute(Laplace_matrix);
	if (solver.info() != Eigen::Success)
	{
		throw std::exception("Compute Matrix Is Error!");
		return;
	}
}

void Paramaterize::Solve()
{
	size_t nV = heMesh->NumVertices();
	Eigen::VectorXd bx(nV), by(nV);
	Eigen::VectorXd x(nV), y(nV);

	bx.setZero(); by.setZero();
	
	for (int i = 0; i < boundary_index.size(); i++)
	{
		V* v = heMesh->Vertices()[boundary_index[i]];
		bx[boundary_index[i]] = boundary_list[i][0];
		by[boundary_index[i]] = boundary_list[i][1];
	}

	x = solver.solve(bx);
	y = solver.solve(by);

	for (size_t i = 0; i < nV; i++)
	{
		if (display_status == kon)
		{
			heMesh->Vertices()[i]->pos.at(0) = x(i);
			heMesh->Vertices()[i]->pos.at(1) = y(i);
			heMesh->Vertices()[i]->pos.at(2) = 0;
		}
		texcoords.push_back(pointf2(x(i), y(i)));
	}

}