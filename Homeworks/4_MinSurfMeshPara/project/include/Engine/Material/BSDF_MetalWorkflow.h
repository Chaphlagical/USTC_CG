#pragma once

#include <Engine/Material/BSDF.h>

#include <Engine/Material/SchlickGGX.h>

namespace Ubpa {
	// Disney
	class BSDF_MetalWorkflow : public BSDF {
	public:
		BSDF_MetalWorkflow(const rgbf& colorFactor = rgbf(1.f), float roughnessFactor = 1.f, float metallicFactor = 1.f)
			: colorFactor(colorFactor), roughnessFactor(roughnessFactor), metallicFactor(metallicFactor),
			albedoTexture(nullptr), metallicTexture(nullptr), aoTexture(nullptr) { }

	public:
		static const Ptr<BSDF_MetalWorkflow> New(const rgbf& colorFactor = rgbf(1.f), float roughnessFactor = 1.f, float metallicFactor = 1.f) {
			return Ubpa::New<BSDF_MetalWorkflow>(colorFactor, roughnessFactor, metallicFactor);
		}

	protected:
		virtual ~BSDF_MetalWorkflow() = default;

	public:
		virtual const rgbf F(const normalf& wo, const normalf& wi, const pointf2& texcoord) override;

		// probability density function
		virtual float PDF(const normalf& wo, const normalf& wi, const pointf2& texcoord) override;

		// PD is probability density
		// return albedo
		virtual const rgbf Sample_f(const normalf& wo, const pointf2& texcoord, normalf& wi, float& PD) override;

		virtual void ChangeNormal(const pointf2& texcoord, const normalf& tangent, normalf& normal) const override;

	private:
		// Fresnel
		static const rgbf Fr(const normalf& w, const normalf& h, const rgbf& albedo, float metallic);

		const rgbf GetAlbedo(const pointf2& texcoord) const;
		float GetMetallic(const pointf2& texcoord) const;
		float GetRoughness(const pointf2& texcoord) const;
		float GetAO(const pointf2& texcoord) const;

	public:
		SchlickGGX sggx;

		rgbf colorFactor;
		Ptr<Image> albedoTexture;

		// 0--1
		float metallicFactor;
		Ptr<Image> metallicTexture;

		// 0--1
		float roughnessFactor;
		Ptr<Image> roughnessTexture;

		Ptr<Image> aoTexture; // ֻ����ʵʱ��Ⱦ

		Ptr<Image> normalTexture;
	};
}
