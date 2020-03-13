#pragma once
#include <UHEMesh/HEMesh.h>
#include <UGM/UGM>
#include <Basic/HeapObj.h>

namespace Ubpa {
	class TriMesh;

	class Paramaterize_Cotangent
	{
	public:
		Paramaterize_Cotangent() {};
		virtual ~Paramaterize_Cotangent() {};

	protected:
		class V;
		class E;
		class P;
		class V : public TVertex<V, E, P> {
		public:
			vecf3 pos;
		};
		class E : public TEdge<V, E, P> { };
		class P :public TPolygon<V, E, P> { };

	public:
		virtual void Init_Matrix(const Ptr<HEMesh<V>>& heMesh) {};
	};
}