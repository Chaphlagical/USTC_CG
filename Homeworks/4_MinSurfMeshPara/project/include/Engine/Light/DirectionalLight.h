#pragma once

#include <Engine/Light/Light.h>

namespace Ubpa {
	// �ֲ�����ϵ�У����� -y
	class DirectionalLight : public Light {
	public:
		DirectionalLight(const rgbf& color = rgbf(1.f), float intensity = 1.0f)
			: color(color), intensity(intensity) { }

	public:
		static const Ptr<DirectionalLight> New(const rgbf& color = rgbf(1.f), float intensity = 1.0f) {
			return Ubpa::New<DirectionalLight>(color, intensity);
		}

	protected:
		virtual ~DirectionalLight() = default;

	public:
		// ���� L ����
		// !!! p��wi ���ڹ�Դ������ռ���
		// @arg0  in���� p �������� distToLight �� PD
		// @arg1 out��wi ָ���Դ��Ϊ��λ����
		// @arg2 out��p �㵽��Դ������ľ���
		// @arg3 out�������ܶ� probability density
		virtual const rgbf Sample_L(const pointf3& p, normalf& wi, float& distToLight, float& PD) const override;

		// �����ܶȺ���
		// !!! p��wi ���ڵƵ�����ռ���
		virtual float PDF(const pointf3& p, const normalf& wi) const override { return 0.f; }

		virtual bool IsDelta() const override { return true; }

	public:
		rgbf color;
		float intensity;
	};
}
