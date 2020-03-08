#pragma once

#include <Engine/Viewer/Ray.h>

#include <Basic/HeapObj.h>

#include <UGM/rgb.h>
#include <UGM/point.h>
#include <UGM/vec.h>
#include <UGM/normal.h>

namespace Ubpa {
	class Light : public HeapObj {
	protected:
		Light() = default;
		virtual ~Light() = default;

	public:
		// ���� L ����
		// !!! p��wi ���ڹ�Դ������ռ���
		// @arg0  in���� p �������� distToLight �� PD
		// @arg1 out��wi ָ���Դ��Ϊ��λ����
		// @arg2 out��p �㵽��Դ������ľ���
		// @arg3 out�������ܶ� probability density
		virtual const rgbf Sample_L(const pointf3& p, normalf& wi, float& distToLight, float& PD) const = 0;

		// �����ܶȺ���
		// !!! p��wi ���ڵƵ�����ռ���
		virtual float PDF(const pointf3& p, const normalf& wi) const = 0;

		virtual bool IsDelta() const = 0;

		// ����Щû�л����κ���������ߵ���
		virtual const rgbf Le(const Ray& ray) const { return rgbf(0.f); }
	};
}
