#pragma once

#include <Engine/Material/MicrofacetDistribution.h>

namespace Ubpa {
	class Beckmann : public MicrofacetDistribution {
	public:
		Beckmann() { SetAlpha(0.5f); }

	public:
		// roughness : 0 - 1
		void SetAlpha(float roughness) {
			roughness = std::max(roughness, (float)1e-3);
			//float x = std::log(roughness);
			//float x2 = x * x;
			//float x3 = x2 * x;
			//float x4 = x2 * x2;
			//alpha = 1.62142f + 0.819955f * x + 0.1734f * x2 +
			//	0.0171201f * x3 + 0.000640711f * x4;
			alpha = roughness * roughness;
		}

	public:
		// ���߷ֲ�����
		virtual float D(const normalf& wh) const override;

		virtual float Lambda(const normalf& w) const override;
		virtual const normalf Sample_wh() const override;

	private:
		float alpha;
	};
}
