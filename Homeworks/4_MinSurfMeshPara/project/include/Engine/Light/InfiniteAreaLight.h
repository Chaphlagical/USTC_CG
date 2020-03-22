#pragma once

#include <Engine/Light/Light.h>

namespace Ubpa {
	class Image;

	class InfiniteAreaLight : public Light {
	public:
		InfiniteAreaLight(Ptr<Image> img = nullptr, const rgbf& colorFactor = rgbf(1.f), float intensity = 1.f)
			: colorFactor(colorFactor), intensity(intensity) {
			SetImg(img);
		}

	public:
		static const Ptr<InfiniteAreaLight> New(Ptr<Image> img, const rgbf& colorFactor = rgbf(1.f), float intensity = 1.f) {
			return Ubpa::New<InfiniteAreaLight>(img, colorFactor, intensity);
		}

	public:
		const PtrC<Image> GetImg() const { return img; }
		void SetImg(Ptr<Image> img);

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

		// ����Щû�л����κ���������ߵ���
		virtual const rgbf Le(const Ray& ray) const override;

	private:
		const rgbf GetColor(const pointf2& texcoord) const;

	public:
		rgbf colorFactor;
		float intensity;

	private:
		Ptr<Image> img;
	};
}
