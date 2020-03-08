#pragma once

#include <Engine/Light/Light.h>

namespace Ubpa {
	// �� -y ��������
	class SpotLight : public Light {
	public:
		SpotLight(const rgbf& color = rgbf(1.f), float intensity = 1.0f, float radius = 10.f,
			float angle = 90.f, float fullRatio = 0.8f)
			: color(color), intensity(intensity), radius(radius),
			angle(angle), fullRatio(fullRatio) { }

	public:
		static const Ptr<SpotLight> New(const rgbf& color = rgbf(1.f), float intensity = 1.0f, float radius = 10.f,
			float angle = 90.f, float fullRatio = 0.8f) {
			return Ubpa::New<SpotLight>(color, intensity, radius, angle, fullRatio);
		}

	protected:
		virtual ~SpotLight() = default;

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

	public:
		float CosHalfAngle() const {
			return cos(to_radian(angle) / 2);
		}
		float CosFalloffAngle() const {
			return cos(to_radian(angle) / 2 * fullRatio);
		}

	private:
		static float Fwin(float d, float radius);
		float Falloff(const normalf& wi) const;

	public:
		rgbf color;
		float intensity;
		float radius;

		float angle; // [1 - 179]
		float fullRatio; // [0 -- 1]
	};
}
