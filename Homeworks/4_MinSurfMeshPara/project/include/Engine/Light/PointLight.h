#pragma once

#include <Engine/Light/Light.h>

namespace Ubpa {
	class PointLight : public Light {
	public:
		PointLight(const rgbf& color = 1.f, float intensity = 1.0f, float radius = 10.f)
			: color(color), intensity(intensity), radius(radius) { }

	public:
		static const Ptr<PointLight> New(const rgbf& color = rgbf(1.f), float intensity = 1.0f, float radius = 10.f) {
			return Ubpa::New<PointLight>(color, intensity, radius);
		}

	protected:
		virtual ~PointLight() = default;

	public:
		const rgbf IlluminancePower() const { return intensity * color; }

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
		virtual float PDF(const pointf3& p, const normalf& wi) const override { return 0; }

		virtual bool IsDelta() const override { return true; }

	private:
		static float Fwin(float d, float radius);

	public:
		// color * intensity = lumminance power
		rgbf color;
		float intensity;
		float radius;
	};
}
