#include <Engine/MeshEdit/Parameterize_ASAP.h>
#include <Eigen/Sparse>

using namespace Ubpa;
using namespace std;

Parameterize_ASAP::Parameterize_ASAP(Ptr<TriMesh> triMesh)
	:Paramaterize_Basic::Paramaterize_Basic(triMesh)
{
	nP = nV = 0;
}

Parameterize_ASAP::~Parameterize_ASAP()
{
	Clear();
}

void Parameterize_ASAP::Parameterization()
{
	clock_t time = clock();
	nP = heMesh->NumPolygons();
	nV = heMesh->NumVertices();
	Local_Flatten();
	Init_Matrix();
	Solve();
	cout << "Time:" <<  clock()- time << endl;
}


coff_map Parameterize_ASAP::Map_Add(coff_map map1, coff_map map2)
{
	coff_map result = map1;
	for (auto iter2 = map2.begin(); iter2 != map2.end(); iter2++)
	{
		if (!result.count(iter2->first))
			result[iter2->first] = iter2->second;
		else
			result[iter2->first] += iter2->second;
	}
	return result;
}

double Parameterize_ASAP::Map_Derivation(coff_map map_, size_t elem)
{
	return map_[elem];
}

coff_map Parameterize_ASAP::Map_Multiply(coff_map map_, double coef)
{
	coff_map result;
	for (auto iter = map_.begin(); iter != map_.end(); iter++)
	{
		result.insert(coff_map::value_type(iter->first, iter->second * coef));
	}
	return result;
}

std::vector<coff_map> Parameterize_ASAP::Get_elem_map(size_t t, size_t i)
{
	std::vector<coff_map> elem_vec;
	coff_map elem_x, elem_y;
	double C1 = 0;
	coff_map C2, C3;
	for (size_t k = 0; k < 3; k++)
	{
		C1 += cotangent_list[t][k] * (delta_x_list[t][k] * delta_x_list[t][k] + delta_y_list[t][k] * delta_y_list[t][k]);
		C2.insert(coff_map::value_type(vertices_index_hemesh[t][k], cotangent_list[t][k] * delta_x_list[t][k]));	// x
		C2.insert(coff_map::value_type(vertices_index_hemesh[t][k] + nV, cotangent_list[t][k] * delta_y_list[t][k]));	//	y
		C2.insert(coff_map::value_type(vertices_index_hemesh[t][(k+1)%3], -cotangent_list[t][k] * delta_x_list[t][k]));	// x
		C2.insert(coff_map::value_type(vertices_index_hemesh[t][(k+1)%3] + nV, -cotangent_list[t][k] * delta_y_list[t][k]));	//	y

		C3.insert(coff_map::value_type(vertices_index_hemesh[t][k], cotangent_list[t][k] * delta_y_list[t][k]));	// x
		C3.insert(coff_map::value_type(vertices_index_hemesh[t][k] + nV, -cotangent_list[t][k] * delta_x_list[t][k]));	//	y
		C3.insert(coff_map::value_type(vertices_index_hemesh[t][(k + 1) % 3], -cotangent_list[t][k] * delta_y_list[t][k]));	// x
		C3.insert(coff_map::value_type(vertices_index_hemesh[t][(k + 1) % 3] + nV, cotangent_list[t][k] * delta_x_list[t][k]));	//	y
	}
	elem_x.insert(coff_map::value_type(vertices_index_hemesh[t][i], 1));
	elem_x.insert(coff_map::value_type(vertices_index_hemesh[t][(i+1)%3], -1));
	elem_y.insert(coff_map::value_type(vertices_index_hemesh[t][i], 1));
	elem_y.insert(coff_map::value_type(vertices_index_hemesh[t][(i + 1) % 3], -1));

	elem_x = Map_Add(elem_x, Map_Multiply(C2, -delta_x_list[t][i] / C1));
	elem_x = Map_Add(elem_x, Map_Multiply(C3, -delta_y_list[t][i] / C1));
	elem_y = Map_Add(elem_y, Map_Multiply(C2, -delta_y_list[t][i] / C1));
	elem_y = Map_Add(elem_y, Map_Multiply(C3, delta_x_list[t][i] / C1));
	
	elem_vec.push_back(elem_x);
	elem_vec.push_back(elem_y);
	for (size_t t = 0; t < 2; t++)
	{
		for (auto it = elem_vec[t].begin(); it != elem_vec[t].end(); it++)
			cout << "(" << it->first << ", " << it->second << ") ";
		cout << endl;
	}


	return elem_vec;
}

void Parameterize_ASAP::Init_Matrix()
{
	vector<Eigen::Triplet<double>> Lij;
	for (size_t n = 0; n < nV; n++)
	{
		coff_map coefn[2];
		for (size_t t = 0; t < nP; t++)
		{
			for (size_t i = 0; i < 3; i++)
			{
				vector<coff_map> elem_map = Get_elem_map(t, i);
				for (size_t k = 0; k < 2; k++)
				{
					coff_map elem = elem_map[k];
					elem = Map_Multiply(Map_Multiply(elem, Map_Derivation(elem, vertices_index_hemesh[t][i]*(k+1))),cotangent_list[t][i]);
					coefn[k] = Map_Add(coefn[k], elem);
				}
			}
		}
		for (size_t k = 0; k < 2; k++)
		{
			for (auto iter = coefn[k].begin(); iter != coefn[k].end(); iter++)
				if((n<2||(n>=nV&&n<nV+2))&&n==iter->first)
					Lij.push_back(Eigen::Triplet<double>(n*(k+1), iter->first, 1));
				else
					Lij.push_back(Eigen::Triplet<double>(n*(k+1), iter->first, iter->second));
		}
	}
	Coef_matrix.resize(2 * nV, 2 * nV);
	Coef_matrix.setZero();
	b_vector.resize(nV * 2);
	b_vector.setZero();
	b_vector(0) = heMesh->Vertices()[0]->pos[0];
	b_vector(0) = heMesh->Vertices()[0]->pos[0];
	b_vector(nV) = heMesh->Vertices()[0]->pos[1];
	b_vector(nV+1) = heMesh->Vertices()[0]->pos[1];

	Coef_matrix.setFromTriplets(Lij.begin(), Lij.end());
	cout << Coef_matrix;
	
}

void Parameterize_ASAP::Solve()
{
	Eigen::VectorXd X(2 * nV + 4 * nP);

	cout << "Solving Equation..." << endl;
	solver.compute(Coef_matrix);
	X = solver.solve(b_vector);
	cout << "Solving complete" << endl;

	for (size_t i = 0; i < nV; i++)
	{
		texcoords.push_back(pointf2(X(i), X(i+nV)));
		heMesh->Vertices()[i]->pos.at(0) = X(i);
		heMesh->Vertices()[i]->pos.at(1) = X(i + nV);
		heMesh->Vertices()[i]->pos.at(2) = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

size_t Parameterize_ASAP::Get_a_index(size_t t)
{
	return 2 * nV + t;
}

size_t Parameterize_ASAP::Get_b_index(size_t t)
{
	return 2 * nV + nP + t;
}

veci2 Parameterize_ASAP::Get_u_index(size_t index)
{
	return veci2(index * 2, index * 2 + 1);
}

bool Parameterize_ASAP::map_update(size_t index, coff_map& map)
{
	if (index > 2)//	not fixed
	{
		if (!map.count(index-3))
		{
			map.insert(coff_map::value_type(index - 3, 0));
			map.insert(coff_map::value_type(index + nV - 6, 0));
		}
		return true;
	}
	else
		return false;	//	fixed
}

std::vector<std::vector<double>> Parameterize_ASAP::Get_delta_list(size_t index)
{
	if (index)
		return delta_y_list;
	else
		return delta_x_list;
}