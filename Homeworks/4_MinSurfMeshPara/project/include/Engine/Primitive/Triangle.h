#pragma once

#include "Shape.h"

namespace Ubpa {
	class TriMesh;

	// ��Ϊ Triangle ������ Mesh������ Mesh �ֿ���ȡ�� Triangle
	// ���Բ��� Triangle ���� Primitive��ֻ�� Mesh ���� Primitive
	class Triangle : public Shape {
		friend class TriMesh;
	public:
		Triangle(unsigned idx0, unsigned idx1, unsigned idx2)
			: mesh(Ptr<TriMesh>(nullptr)), idx{ idx0, idx1, idx2 } { }

	public:
		static const Ptr<Triangle> New(unsigned idx0, unsigned idx1, unsigned idx2) {
			return Ubpa::New<Triangle>(idx0, idx1, idx2);
		}

	protected:
		virtual ~Triangle() = default;

	public:
		const Ptr<TriMesh> GetMesh() const { return mesh.lock(); }
		virtual const bboxf3 GetBBox() const override;
		virtual const Ptr<Primitive> GetPrimitive() override {
			return CastTo<Primitive>(mesh.lock());
		}

	public:
		float GetArea() const;

	public:
		unsigned idx[3]; // index into the mesh attribute arrays

	private:
		WPtr<TriMesh> mesh;
	};
}
