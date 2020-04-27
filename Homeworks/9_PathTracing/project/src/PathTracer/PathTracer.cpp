#include "PathTracer.h"

#include <UBL/Image.h>

#include <iostream>

#include <thread>

#include <algorithm>

using namespace Ubpa;
using namespace std;

struct prob
{
	float p;
	float u;
	int k;
};
std::vector<prob> table;

static void Init_AliasTable(const std::vector<float>& probs);
static float Alias_PDF(const vecf3& dir, const EnvLight* light);
static std::tuple<rgbf, vecf3, float> Alias_Sample_L(const normalf& n, const EnvLight* light);

PathTracer::PathTracer(const Scene* scene, const SObj* cam_obj, Image* img)
	: scene{ scene },
	bvh{ const_cast<Scene*>(scene) },
	img{ img },
	cam{ cam_obj->Get<Cmpt::Camera>() },
	ccs{ cam->GenCoordinateSystem(cam_obj->Get<Cmpt::L2W>()->value)}
{
	IntersectorVisibility::Instance();
	IntersectorClosest::Instance();

	scene->Each([this](const Cmpt::Light* light) ->bool {
		if (!vtable_is<EnvLight>(light->light.get()))
			return true; // continue

		env_light = static_cast<const EnvLight*>(light->light.get());
		return false; // stop
	});

	// TODO: preprocess env_light here
	/*int h = env_light->texture->img.get()->height;
	int w = env_light->texture->img.get()->width;

	cout << "h: " << h << endl << "w: " << w << endl;
	
	vector<float> probs(w* h);
	float sum_prob = 0;
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			probs[j + w * i] = env_light->texture->img.get()->At(j, i).to_rgb().illumination();
			sum_prob += probs[j + w * i];
		}
	}
	for (auto& i : probs)
		i /= sum_prob;
	Init_AliasTable(probs);*/
	
}

void Init_AliasTable(const std::vector<float>& probs)
{
	int num = probs.size();
	table.clear();
	table.resize(num);
	
	vector<int> overfull;
	vector<int> underfull;
	
	for (int i = 0; i < num; i++)
	{
		table[i].k = i;
		table[i].p = probs[i];
		table[i].u = num * probs[i];

		if (table[i].u < 1)underfull.push_back(i);
		if (table[i].u > 1)overfull.push_back(i);
	}

	while (overfull.empty() && underfull.empty())
	{
		int i = overfull.back();
		overfull.pop_back();

		int j = underfull.back();
		underfull.pop_back();

		table[i].u -= 1 - table[j].u;
		table[i].k = i;

		if (table[i].u > 1)overfull.push_back(i);
		if (table[i].u < 1)underfull.push_back(i);
	}
}

float Alias_PDF(const vecf3& dir, const EnvLight* light)
{
	auto wi = dir.normalize().cast_to<normalf>();
	int h = light->texture->img.get()->height;
	int w = light->texture->img.get()->width;

	float phi = std::atan2(-wi[0], -wi[2]) + PI<float>;
	float theta = acos(wi[1]);

	auto u = phi / (2.f * PI<float>);
	auto v = theta / PI<float>;
	auto texcoord = pointf2(u, v);

	auto x = (size_t)std::round(w * u - 0.5);
	auto y = (size_t)std::round(h * v - 0.5);

	float p_img = table[x + w * y].p;

	return p_img / (2.f * PI<float> * PI<float> * sin(theta));
}

tuple<rgbf, vecf3, float> Alias_Sample_L(const normalf& n, const EnvLight* light)
{
	int h = light->texture->img.get()->height;
	int w = light->texture->img.get()->width;

	float nx = rand01<float>() * (float)(h * w);
	int i_ = floor(nx) + 1;
	float y_ = nx + 1 - i_;
	int p_idx = y_ < table[i_].u ? i_ : table[i_].k;
	int x = p_idx % w;
	int y = (int)((float)(p_idx - x) / (float)w);

	float u = (x + rand01<float>()) / w;
	float v = (y + rand01<float>())/h;

	float theta = v * PI<float>;
	float phi = u * 2.f * PI<float>;

	vecf3 wi = (sin(theta) * sin(phi), cos(theta), sin(theta) * cos(phi));

	float p_img = table[x + w * y].p;
	float env_pd = p_img / (2.f * PI<float> * PI<float> * sin(theta));

	return { light->Radiance(wi), wi.normalize(),env_pd };

}

void PathTracer::Run() {
	img->SetAll(0.f);

	const size_t spp = 1024; // samples per pixel

#ifdef NDEBUG
	const size_t core_num = std::thread::hardware_concurrency();
	auto work = [this, core_num, spp](size_t id) {
		for (size_t j = id; j < img->height; j += core_num) {
			for (size_t i = 0; i < img->width; i++) {
				for (size_t k = 0; k < spp; k++) {
					float u = (i + rand01<float>() - 0.5f) / img->width;
					float v = (j + rand01<float>() - 0.5f) / img->height;
					rayf3 r = cam->GenRay(u, v, ccs);
					rgbf Lo;
					do { Lo = Shade(IntersectorClosest::Instance().Visit(&bvh, r), -r.dir, true); }
					while (Lo.has_nan());
					img->At<rgbf>(i, j) += Lo / float(spp);
				}
			}
			float progress = (j + 1) / float(img->height);
			cout << progress << endl;
		}
	};
	vector<thread> workers;
	for (size_t i = 0; i < core_num; i++)
		workers.emplace_back(work, i);
	for (auto& worker : workers)
		worker.join();
#else
	for (size_t j = 0; j < img->height; j++) {
		for (size_t i = 0; i < img->width; i++) {
			for (size_t k = 0; k < spp; k++) {
				float u = (i + rand01<float>() - 0.5f) / img->width;
				float v = (j + rand01<float>() - 0.5f) / img->height;
				rayf3 r = cam->GenRay(u, v, ccs);
				rgbf Lo;
				do { Lo = Shade(IntersectorClosest::Instance().Visit(&bvh, r), -r.dir, true); }
				while (Lo.has_nan());
				img->At<rgbf>(i, j) += Lo / spp;
			}
		}
		float progress = (j + 1) / float(img->height);
		cout << progress << endl;
	}
#endif
	cout << "Finish!" << endl;
}

rgbf PathTracer::Shade(const IntersectorClosest::Rst& intersection, const vecf3& wo, bool last_bounce_specular) {
	// TODO: HW9 - Trace
	// [ Tips ]
	// - EnvLight::Radiance(<direction>), <direction> is pointing to environment light
	// - AreaLight::Radiance(<uv>)
	// - rayf3: point, dir, tmin, **tmax**
	// - IntersectorVisibility::Instance().Visit(&bvh, <rayf3>)
	//   - tmin = EPSILON<float>
	//   - tmax = distance to light - EPSILON<float>
	// - IntersectorCloest::Instance().Visit(&bvh, <rayf3>)
	//   - tmin as default (EPSILON<float>)
	//   - tmax as default (FLT_MAX)
	//
	// struct IntersectorClosest::Rst {
	//	 bool IsIntersected() const noexcept { return sobj != nullptr; }
	//	 const SObj* sobj{ nullptr }; // intersection sobj
	//	 pointf3 pos; // intersection point's position
	//	 pointf2 uv; // texcoord
	//   normalf n; // normal, normalized
	//	 vecf3 tangent; // perpendicular to normal, normalized
	// };

	constexpr rgbf error_color = rgbf{ 1.f,0.f,1.f };
	constexpr rgbf todo_color = rgbf{ 0.f,1.f,0.f };
	constexpr rgbf zero_color = rgbf{ 0.f,0.f,0.f };

	if (!intersection.IsIntersected()) {
		if (last_bounce_specular && env_light != nullptr) {
			// TODO: environment light

			return env_light->Radiance(-wo);
		}
		else
			return zero_color;
	}
	
	if (!intersection.sobj->Get<Cmpt::Material>()) {
		auto light = intersection.sobj->Get<Cmpt::Light>();
		if(!light) return error_color;

		if (last_bounce_specular) { // avoid double-count
			auto area_light = dynamic_cast<const AreaLight*>(light->light.get());
			if (!area_light) return error_color;

			// TODO: area light
			return area_light->Radiance(intersection.uv);
		}else
			return zero_color;
	}
	
	rgbf L_dir{ 0.f };
	rgbf L_indir{ 0.f };

	scene->Each([=, &L_dir](const Cmpt::Light* light, const Cmpt::L2W* l2w, const Cmpt::SObjPtr* ptr) {
		// TODO: L_dir += ...
		// - use PathTracer::BRDF to get BRDF value
		SampleLightResult sample_light_rst = SampleLight(intersection, wo, light, l2w, ptr);
		if (sample_light_rst.pd <= 0)
			return;
		if (sample_light_rst.is_infinity) {
			// TODO: L_dir of environment light
			// - only use SampleLightResult::L, n, pd
			// - SampleLightResult::x is useless

			vecf3 wi = - sample_light_rst.n.cast_to<vecf3>();
			float cos_theta = intersection.n.cast_to<vecf3>().dot(wi);
			rgbf f_r = PathTracer::BRDF(intersection, wi.normalize(), wo.normalize());
			rayf3 r(intersection.pos, wi, EPSILON<float>);
			bool visibility = IntersectorVisibility::Instance().Visit(&bvh, r);
			L_dir += sample_light_rst.L * f_r * visibility * abs(cos_theta) / sample_light_rst.pd;
		}
		else {
			// TODO: L_dir of area light
			//intersection.pos: x
			//intersection.n: n(x)
			//sample_light_rst.n: n(y)
			//sample_light_rst.x: y
			// - Intersectors::visibility.Visit(&bvh, <rayf3>)
			//   - tmin = EPSILON<float>
			//   - tmax = distance to light - EPSILON<float>

			vecf3 wi = sample_light_rst.x - intersection.pos;
			//vecf3 wi = intersection.pos - sample_light_rst.x;
			float cos_theta_yx = wi.dot(sample_light_rst.n.cast_to<vecf3>() / (wi.norm()));
			float cos_theta_xy = (-wi).dot(intersection.n.cast_to<vecf3>() / (wi.norm()));
			rgbf f_r = PathTracer::BRDF(intersection, wi.normalize(), wo.normalize());
			rayf3 r(sample_light_rst.x, -wi, EPSILON<float>,1 - EPSILON<float>);
			bool visibility = IntersectorVisibility::Instance().Visit(&bvh, r);
			float G = abs(cos_theta_xy * cos_theta_yx) / wi.norm2();
			if(cos_theta_yx<0)
				L_dir += sample_light_rst.L * f_r * G * visibility / sample_light_rst.pd;
			
		}
	});

	// TODO: Russian Roulette
	// - rand01<float>() : random in [0, 1)
	if (rand01<float>()>0.8)
		return zero_color;
	// TODO: recursion
	// - use PathTracer::SampleBRDF to get wi and pd (probability density)
	// wi may be **under** the surface
	// - use PathTracer::BRDF to get BRDF value

	std::tuple<vecf3, float> t_ = PathTracer::SampleBRDF(intersection, wo);
	auto wi = std::get<0>(t_);
	auto pd = std::get<1>(t_);
	rgbf f_r = PathTracer::BRDF(intersection, wi.normalize(), wo.normalize());
	rayf3 r(intersection.pos, wi, EPSILON<float>);
	float cos_theta = intersection.n.cast_to<vecf3>().dot(wi) / wi.norm();

	L_indir = Shade(IntersectorClosest::Instance().Visit(&bvh, r), -wi, false) * f_r * abs(cos_theta) / pd;

	// TODO: combine L_dir and L_indir
	if (isnan(L_indir[0]) && L_indir[1] && L_indir[2])
		L_indir = zero_color;
	return L_dir+L_indir;
}

PathTracer::SampleLightResult PathTracer::SampleLight(IntersectorClosest::Rst intersection, const vecf3& wo, const Cmpt::Light* light, const Cmpt::L2W* l2w, const Cmpt::SObjPtr* ptr) {
	PathTracer::SampleLightResult rst;
	if (vtable_is<AreaLight>(light->light.get())) {
		auto area_light = static_cast<const AreaLight*>(light->light.get());
		auto geo = ptr->value->Get<Cmpt::Geometry>();
		if (!geo) return rst; // invalid
		if (!vtable_is<Square>(geo->primitive.get())) return rst; // not support

		auto Xi = uniform_in_square<float>(); // [0, 1] x [0, 1]
		pointf3 pos_light_space{ 2 * Xi[0] - 1, 0, 2 * Xi[1] - 1 };
		scalef3 s = l2w->WorldScale();
		float area = s[0] * s[1] * Square::area;

		rst.L = area_light->Radiance(Xi.cast_to<pointf2>());
		rst.pd = 1.f / area;
		rst.x = l2w->value * pos_light_space;
		rst.is_infinity = false;
		rst.n = l2w->UpInWorld().cast_to<normalf>();
	}
	else if (vtable_is<EnvLight>(light->light.get())) {
		rst.is_infinity = true;

		auto mat = intersection.sobj->Get<Cmpt::Material>();
		if (!mat) return rst; // invalid
		auto brdf = dynamic_cast<const stdBRDF*>(mat->material.get());
		if (!brdf) return rst; // not support

		// multi-importance sampling, MIS

		auto env_light = static_cast<const EnvLight*>(light->light.get());

		float metalness = brdf->Metalness(intersection.uv);
		float roughness = brdf->Roughness(intersection.uv);
		float lambda = metalness * (1 - stdBRDF::Alpha(roughness)); // 0 - 1
		float p_mat = 1 / (2 - lambda); // 0.5 - 1
		float pd_mat, pd_env;
		vecf3 wi;
		rgbf Le;

		if (rand01<float>() < p_mat) {
			tie(wi, pd_mat) = SampleBRDF(intersection, wo);
			Le = env_light->Radiance(wi);
			pd_env = env_light->PDF(wi); // TODO: use your PDF
			//pd_env = Alias_PDF(wi, env_light);
		}
		else {
			tie(Le, wi, pd_env) = env_light->Sample(intersection.n); // TODO: use your sampling method
			//tie(Le, wi, pd_env) = Alias_Sample_L(intersection.n, env_light);
			matf3 surface_to_world = svecf::TBN(intersection.n.cast_to<vecf3>(), intersection.tangent);
			matf3 world_to_surface = surface_to_world.inverse();
			svecf s_wo = (world_to_surface * wo).cast_to<svecf>();
			svecf s_wi = (world_to_surface * wi).cast_to<svecf>();
			rgbf albedo = brdf->Albedo(intersection.uv);
			pd_mat = brdf->PDF(albedo, metalness, roughness, s_wi, s_wo);
		}

		rst.L = Le;
		rst.n = -wi.cast_to<normalf>();
		rst.pd = p_mat * pd_mat + (1 - p_mat) * pd_env;
		rst.x = pointf3{ std::numeric_limits<float>::max() };
	}
	return rst;
}

std::tuple<vecf3, float> PathTracer::SampleBRDF(IntersectorClosest::Rst intersection, const vecf3& wo) {
	auto mat = intersection.sobj->Get<Cmpt::Material>();
	if (!mat) return { vecf3{0.f}, 0.f };
	auto brdf = dynamic_cast<const stdBRDF*>(mat->material.get());
	if (!brdf) return { vecf3{0.f}, 0.f };

	matf3 surface_to_world = svecf::TBN(intersection.n.cast_to<vecf3>(), intersection.tangent);
	matf3 world_to_surface = surface_to_world.inverse();
	svecf s_wo = (world_to_surface * wo).cast_to<svecf>();

	rgbf albedo = brdf->Albedo(intersection.uv);
	float metalness = brdf->Metalness(intersection.uv);
	float roughness = brdf->Roughness(intersection.uv);

	auto [s_wi, pdf] = brdf->Sample(albedo, metalness, roughness, s_wo);
	vecf3 wi = surface_to_world * s_wi;

	return { wi,pdf };
}

rgbf PathTracer::BRDF(IntersectorClosest::Rst intersection, const vecf3& wi, const vecf3& wo) {
	auto mat = intersection.sobj->Get<Cmpt::Material>();
	if (!mat) return rgbf{ 1.f,0.f,1.f };
	auto brdf = dynamic_cast<const stdBRDF*>(mat->material.get());
	if (!brdf) return rgbf{ 1.f,0.f,1.f };

	matf3 surface_to_world = svecf::TBN(intersection.n.cast_to<vecf3>(), intersection.tangent);
	matf3 world_to_surface = surface_to_world.inverse();
	svecf s_wi = (world_to_surface * wi).cast_to<svecf>();
	svecf s_wo = (world_to_surface * wo).cast_to<svecf>();

	rgbf albedo = brdf->Albedo(intersection.uv);
	float metalness = brdf->Metalness(intersection.uv);
	float roughness = brdf->Roughness(intersection.uv);

	return brdf->BRDF(albedo, metalness, roughness, s_wi, s_wo);
}
