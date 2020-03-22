# include <Engine/MeshEdit/Paramaterize_Basic.h>
#include <Eigen/Dense>
#include <Eigen/Sparse>

namespace Ubpa
{
	class TriMesh;
	
	class Parameterize_ASAP_GL :
		public Paramaterize_Basic
	{
	public:
		Parameterize_ASAP_GL(Ptr<TriMesh> triMesh);
		~Parameterize_ASAP_GL();

	private:
		void Parameterization();
		void Local_Phase();



	};


}