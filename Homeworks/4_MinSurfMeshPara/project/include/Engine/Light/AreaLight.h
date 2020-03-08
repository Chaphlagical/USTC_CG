#pragma once

#include <Engine/Light/Light.h>
#include <Basic/Sampler/UniformGridSampler2D.h>

namespace Ubpa {
	// ����
	class AreaLight : public Light {
	public:
		AreaLight(const rgbf& color = rgbf(1.f), float intensity = 1.f, float width = 1.f, float height = 1.f)
			: color(color), intensity(intensity), width(width), height(height) { }

	public:
		static const Ptr<AreaLight> New(const rgbf& color = rgbf(1.f), float intensity = 1.f, float width = 1.f, float height = 1.f) {
			return Ubpa::New<AreaLight>(color, intensity, width, height);
		}

	public:
		float Area() const { return width * height; }
		const rgbf LuminancePower() const { return intensity * color; }
		const rgbf Luminance() const { return LuminancePower() / (Area() * PI<float>); }

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
		virtual float PDF(const pointf3& p, const normalf& wi) const override;

		virtual bool IsDelta() const override { return false; }

	public:
		rgbf color;
		float intensity;
		float width;
		float height;

	private:
		// dirToLight ������ ��λ����
		bool Hit(const pointf3& p, const vecf3& dirToLight, pointf3& hitPos) const;

		UniformGridSampler2D sampler;
	};
}
