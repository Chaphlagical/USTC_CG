c

using namespace Ubpa;
using namespace std;

Paramaterize_Uniform::Paramaterize_Uniform()
{

}

Paramaterize_Uniform::~Paramaterize_Uniform()
{

}

void Paramaterize_Uniform::Init_Matrix(const Ptr<HEMesh<V>>& heMesh, Eigen::SparseMatrix<double>& Laplace_matrix)
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