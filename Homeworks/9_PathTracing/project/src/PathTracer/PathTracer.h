#pragma once

#include <UScene/UScene.h>

#include <vector>

namespace Ubpa {

	class PathTracer {
	public:
		PathTracer(const Scene* scene, const SObj* cam_obj, Image* img);

		void Run();

	private:
		struct Intersectors {
			IntersectorClosest clostest;
			IntersectorVisibility visibility;
		};

		rgbf Shade(const Intersectors& intersectors, const IntersectorClosest::Rst& intersection, const vecf3& wo, bool last_bounce_specular = false);

		struct SampleLightResult {
			rgbf L{ 0.f }; // light radiance
			float pd{ 0.f }; // probability density
			normalf n{ 0.f }; // normalize normal
			pointf3 x{ 0.f }; // position on light
			bool is_infinity{ false }; // infinity distance
		};
		static SampleLightResult SampleLight(const Cmpt::Light* light, const Cmpt::L2W* l2w, const Cmpt::SObjPtr* ptr);

		// wi (normalized), pd (probability density)
		static std::tuple<vecf3, float> SampleBRDF(IntersectorClosest::Rst intersection, const vecf3& wo);
		static rgbf BRDF(IntersectorClosest::Rst intersection, const vecf3& wi, const vecf3& wo);

		const Scene* const scene;
		const EnvLight* env_light{ nullptr };
		Image* const img;

		BVH bvh;

		const Cmpt::Camera* const cam;
		const Cmpt::Camera::CoordinateSystem ccs;
	};
}