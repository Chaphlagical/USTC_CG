#pragma once

#include "Primitive.h"

#include <UGM/point.h>
#include <UGM/normal.h>

namespace Ubpa {
	class Sphere : public Primitive {
	public:
		Sphere() = default;
	public:
		static const Ptr<Sphere> New() { return Ubpa::New<Sphere>(); }

	protected:
		virtual ~Sphere() = default;

	public:
		virtual const bboxf3 GetBBox() const override {
			return bboxf3(pointf3(-1.f, -1.f, -1.f), pointf3(1.f, 1.f, 1.f));
		}

	public:
		struct SphereCoord {
			SphereCoord(float theta, float phi) : theta(theta), phi(phi) {}

			SphereCoord(const pointf2& texcoord)
				: SphereCoord(texcoord[1] * PI<float>, texcoord[0] * 2.f * PI<float>) { }

			const normalf ToDir() const;

			float theta;
			float phi;
		};

		// ������תΪ��������
		// theta ��Ӧ v���� 0 �� 1
		// phi ��Ӧ u��z ��Ϊ 0��˳���ַ�������
		static const pointf2 TexcoordOf(const normalf& normal);

		// ������תΪ����
		// u ����
		static const normalf TangentOf(const normalf& normal);
	};
}
