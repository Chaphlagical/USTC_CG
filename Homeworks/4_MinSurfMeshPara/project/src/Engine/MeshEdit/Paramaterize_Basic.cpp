# include <Engine/MeshEdit/Paramaterize_Basic.h>

#include <Engine/MeshEdit/MinSurf.h>

#include <Engine/Primitive/TriMesh.h>

#include <Engine/MeshEdit/Paramaterize.h>

using namespace Ubpa;
using namespace std;


Paramaterize_Basic::Paramaterize_Basic(Ptr<TriMesh> triMesh)
{
	heMesh = make_shared<HEMesh<V>>();
	display_status = false;
	fixed_num.push_back(0);
	iteration_times = 1;
	Init(triMesh);
}

Paramaterize_Basic::~Paramaterize_Basic()
{
}

void Paramaterize_Basic::Clear() {
	heMesh->Clear();
	triMesh = nullptr;
}

bool Paramaterize_Basic::Init(Ptr<TriMesh> triMesh)
{
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

void Paramaterize_Basic::Set_Iteration(size_t t)
{
	iteration_times = t;
}


bool Paramaterize_Basic::Run() {
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::MinSurf::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	Parameterization();

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

void Paramaterize_Basic::Local_Flatten()
{
	for (size_t t = 0; t < nP; t++)
	{
		P* polygon = heMesh->Polygons()[t];
		vector<pointf2> p_list;
		vector<double> cot_list;
		vector<double> delta_x;
		vector<double> delta_y;
		double cosine[3] = { 0 };
		vecf3 vec0 = polygon->BoundaryVertice()[0]->pos;
		vecf3 vec1 = polygon->BoundaryVertice()[1]->pos;
		vecf3 vec2 = polygon->BoundaryVertice()[2]->pos;
		//cout << vec0 << " " << vec1 << " " << vec2 << endl;
		index_map temp_map;
		vector<size_t> temp_vec;
		for (size_t i = 0; i < 3; i++)
		{
			size_t index = heMesh->Index(polygon->BoundaryVertice()[i]);
			temp_map.insert(index_map::value_type(index, i));
			temp_vec.push_back(index);
		}
		vertices_index_polygon.push_back(temp_map);
		vertices_index_hemesh.push_back(temp_vec);

		cosine[0] = vecf3::cos_theta(vec1 - vec0, vec2 - vec0);
		cosine[1] = vecf3::cos_theta(vec0 - vec1, vec2 - vec1);
		cosine[2] = vecf3::cos_theta(vec0 - vec2, vec1 - vec2);

		p_list.push_back(pointf2(0, 0));
		p_list.push_back(pointf2((vec1 - vec0).norm(), 0));
		p_list.push_back(pointf2((vec2 - vec0).norm() * cosine[0], (vec2 - vec0).norm() * sqrt(1 - cosine[0] * cosine[0])));

		for (int i = 0; i < 3; i++)
		{
			delta_x.push_back((double)p_list[i % 3][0] - (double)p_list[(i + 1) % 3][0]);
			delta_y.push_back((double)p_list[i % 3][1] - (double)p_list[(i + 1) % 3][1]);
			cot_list.push_back(cosine[i] / sqrt(1 - cosine[i] * cosine[i]));
			if (t == 0)
			{
				fixed_pt.insert(pt_map::value_type(heMesh->Index(polygon->BoundaryVertice()[i]), p_list[i]));
			}
		}
		delta_x_list.push_back(delta_x);
		delta_y_list.push_back(delta_y);
		vertices_list.push_back(p_list);
		cotangent_list.push_back(cot_list);

	}
}

void Paramaterize_Basic::Set_Display()
{
	display_status = true;
}

void Paramaterize_Basic::Param_Init()
{
	nP = heMesh->NumPolygons();
	nV = heMesh->NumVertices();
	Local_Flatten();

	auto paramaterize = Paramaterize::New(triMesh);
	paramaterize->Set_Cotangent_Method();
	paramaterize->Set_Boundary_Circle();
	paramaterize->Paramaterization();
	u_list = paramaterize->Get_texcoord();

	for (size_t t = 0; t < nP; t++)
	{
		vector<pointf2> delta_u;
		for (size_t i = 0; i < 3; i++)
		{
			pointf2 p1, p2;
			p1 = u_list[vertices_index_hemesh[t][i]];
			p2 = u_list[vertices_index_hemesh[t][(i + 1) % 3]];
			delta_u.push_back(pointf2(p1[0] - p2[0], p1[1] - p2[1]));
		}
		delta_u_list.push_back(delta_u);
	}
}



void Paramaterize_Basic::B_Init()
{
	Eigen::Vector2d x;
	Eigen::Vector2d temp_vec;
	B.resize(nV, 2);
	B.setZero();

	for (size_t i = 0; i < nV; i++)
	{
		x.setZero();
		temp_vec.setZero();
		V* vi = heMesh->Vertices()[i];
		auto edge_ = vi->HalfEdge();
		do
		{
			V* vj = edge_->End();
			size_t tij, tji;
			double cotij = 0, cotji = 0;
			size_t j = heMesh->Index(vj);


			P* pij = edge_->Polygon();
			P* pji = edge_->Pair()->Polygon();

			if (pij != NULL)
			{
				tij = heMesh->Index(edge_->Polygon());
				cotij = cotangent_list[tij][Get_opposite_index(vertices_index_polygon[tij][i], vertices_index_polygon[tij][j])];

				pointf2 xi = vertices_list[tij][vertices_index_polygon[tij][i]];
				pointf2 xj = vertices_list[tij][vertices_index_polygon[tij][j]];
				x << xi[0] - xj[0], xi[1] - xj[1];
				temp_vec += cotij * L_matrix_list[tij] * x;
			}
			if (pji != NULL)
			{
				tji = heMesh->Index(edge_->Pair()->Polygon());
				cotji = cotangent_list[tji][Get_opposite_index(vertices_index_polygon[tji][i], vertices_index_polygon[tji][j])];
				pointf2 xi = vertices_list[tji][vertices_index_polygon[tji][i]];
				pointf2 xj = vertices_list[tji][vertices_index_polygon[tji][j]];
				x << xi[0] - xj[0], xi[1] - xj[1];
				temp_vec += cotji * L_matrix_list[tji] * x;
			}
			B(i, 0) = temp_vec(0);
			B(i, 1) = temp_vec(1);
			edge_ = edge_->Pair()->Next();
		} while (edge_ != vi->HalfEdge());
	}

	for (size_t i = 0; i < fixed_num.size(); i++)
	{
		B(fixed_num[i], 0) = u_list[fixed_num[i]][0];
		B(fixed_num[i], 1) = u_list[fixed_num[i]][1];
	}
}

void Paramaterize_Basic::Solve()
{
	Eigen::MatrixXd X(nV, 2);

	X = solver.solve(B);

	texcoords.clear();

	double Xmax=-INFINITY, Xmin=INFINITY, Ymax=-INFINITY, Ymin=INFINITY;

	for (size_t i = 0; i < nV; i++)
	{
		if (X(i, 0) > Xmax)
			Xmax = X(i, 0);
		if (X(i, 0) < Xmin)
			Xmin = X(i, 0);
		if (X(i, 1) > Ymax)
			Ymax = X(i, 1);
		if (X(i, 1) < Ymin)
			Ymin = X(i, 1);
	}

	for (size_t i = 0; i < nV; i++)
	{
		texcoords.push_back(pointf2((X(i, 0) - Xmin) / (Xmax - Xmin), (X(i, 1) - Ymin) / (Ymax - Ymin)));
		if (display_status)
		{
			heMesh->Vertices()[i]->pos.at(0) = (X(i, 0) - Xmin) / (Xmax - Xmin);
			heMesh->Vertices()[i]->pos.at(1) = (X(i, 1) - Ymin) / (Ymax - Ymin);
			heMesh->Vertices()[i]->pos.at(2) = 0;
		}

	}

	u_list.clear();
	u_list = texcoords;
	for (size_t t = 0; t < nP; t++)
	{
		vector<pointf2> delta_u;
		for (size_t i = 0; i < 3; i++)
		{
			pointf2 p1, p2;
			p1 = u_list[vertices_index_hemesh[t][i]];
			p2 = u_list[vertices_index_hemesh[t][(i + 1) % 3]];
			delta_u.push_back(pointf2(p1[0] - p2[0], p1[1] - p2[1]));
		}
		delta_u_list.push_back(delta_u);
	}

}

void Paramaterize_Basic::Matrix_Init()
{
	vector<Eigen::Triplet<double>> Lij;


	for (size_t i = 0; i < nV; i++)
	{
		double u_temp = 0;
		V* vi = heMesh->Vertices()[i];
		auto edge_ = vi->HalfEdge();

		do
		{
			V* vj = edge_->End();
			size_t tij, tji;
			double cotij = 0, cotji = 0;
			size_t j = heMesh->Index(vj);


			P* pij = edge_->Polygon();
			P* pji = edge_->Pair()->Polygon();

			if (pij != NULL)
			{
				tij = heMesh->Index(edge_->Polygon());
				cotij = cotangent_list[tij][Get_opposite_index(vertices_index_polygon[tij][i], vertices_index_polygon[tij][j])];
			}
			if (pji != NULL)
			{
				tji = heMesh->Index(edge_->Pair()->Polygon());
				cotji = cotangent_list[tji][Get_opposite_index(vertices_index_polygon[tji][i], vertices_index_polygon[tji][j])];
			}
			u_temp += cotij + cotji;
			Lij.push_back(Eigen::Triplet<double>(i, j, -cotij - cotji));
			edge_ = edge_->Pair()->Next();
		} while (edge_ != vi->HalfEdge());
		bool flag = true;
		for (size_t k = 0; k < fixed_num.size(); k++)
		{
			if (fixed_num[k] == i)
			{
				flag = false;
				break;
			}
		}
		if (flag)
			Lij.push_back(Eigen::Triplet<double>(i, i, u_temp));
	}

	Coef_matrix.resize(nV, nV);
	Coef_matrix.setFromTriplets(Lij.begin(), Lij.end());
	for (size_t i = 0; i < fixed_num.size(); i++)
		Coef_matrix.insert(fixed_num[i], fixed_num[i]) = 1.0;

	Coef_matrix.makeCompressed();
	solver.compute(Coef_matrix);

}

size_t Paramaterize_Basic::Get_opposite_index(size_t i, size_t j)
{
	return 3 - i - j;
}

void Paramaterize_Basic::Set_Fixed_num(vector<size_t> t)
{
	for (size_t i = 0; i < t.size(); i++)
	{
		if (t[i] != 0)
			t.push_back(i);
	}
}