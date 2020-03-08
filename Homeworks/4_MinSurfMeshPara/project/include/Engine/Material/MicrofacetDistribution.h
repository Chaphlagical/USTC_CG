#pragma once

#include <UGM/normal.h>
#include <Engine/Material/SurfCoord.h>

namespace Ubpa {
	class MicrofacetDistribution {
	public:
		// ���߷ֲ�����
		virtual float D(const normalf& wh) const = 0;

		virtual const normalf Sample_wh() const = 0;

		virtual float G1(const normalf& w) const {
			return 1.f / (1.f + Lambda(w));
		}

		// ���κ���
		virtual float G(const normalf& wo, const normalf& wi, const normalf& wh) const {
			if (!SurfCoord::IsVisible(wo, wh) || !SurfCoord::IsVisible(wi, wh))
				return 0.f;

			return 1.f / (1.f + Lambda(wo) + Lambda(wi));
		}

		// ���κ��� Smith
		virtual float G_Smith(const normalf& wo, const normalf& wi, const normalf& wh) const {
			if (!SurfCoord::IsVisible(wo, wh) || !SurfCoord::IsVisible(wi, wh))
				return 0.f;

			return G1(wo) * G1(wi);
		}

		// ����ĸ����ǹ��� wh ��
		// path tracing ����Ҫ�ĸ���Ӧ�ǹ��� wi ��
		float PDF(const normalf& wh) const {
			return D(wh) * abs(wh[2]);
		}

	protected:
		virtual float Lambda(const normalf& w) const = 0;
	};
}
