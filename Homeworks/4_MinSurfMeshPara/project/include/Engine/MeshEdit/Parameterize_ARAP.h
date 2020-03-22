# include <Engine/MeshEdit/Paramaterize_Basic.h>
#include <Eigen/Dense>
#include <Eigen/Sparse>

namespace Ubpa
{
	class TriMesh;
	
	class Parameterize_ARAP :
		public Paramaterize_Basic
	{
	public:
		Parameterize_ARAP(Ptr<TriMesh> triMesh);
		~Parameterize_ARAP();

	private:
		void Parameterization();
		void Local_Phase();



	};


}