#pragma once

#include <Engine/Scene/Component.h>
#include <Engine/Scene/CmptSimu/MassSpring.h>
#include <vector>


namespace Ubpa {
	class CmptSimulate : public Component {
	public:
		/*CmptSimulate(
			Ptr<SObj> sobj, Ptr<Simulate> simu,float stiff = 1000.0f)
			: Component(sobj), simulate(simu),stiffness(stiff) { }*/
		CmptSimulate(
			Ptr<SObj> sobj = nullptr, Ptr<Primitive> primitive = nullptr)
			: Component(sobj), primitive(primitive) { }


	public:
		/*static const Ptr<CmptSimulate> New(Ptr<SObj> sobj, Ptr<Simulate> simu, float stiff = 1000.0f) {
			return Ubpa::New<CmptSimulate>(sobj, simu, stiff);
		}*/
		static const  Ptr<CmptSimulate> New(Ptr<SObj> sobj, Ptr<Primitive> primitive) {
			return Ubpa::New<CmptSimulate>(sobj, primitive);
		}
	protected:
		virtual ~CmptSimulate() = default;

	public:
		void Init(std::vector<unsigned>& fix) { SetFix(fix); };
		// theta is in radian

	public:
		const std::vector<unsigned>& GetFix() const { return fix_id; };
		float GetStiff() const { return CastTo<MassSpring>(primitive)->GetSimu()->GetStiff(); };
		size_t GetIndex() const { return CastTo<MassSpring>(primitive)->GetSimu()->GetIndex(); };
		size_t GetMaxIndex() const { return CastTo<MassSpring>(primitive)->GetSimu()->GetMaxIndex(); };
		Ptr<Primitive> GetMesh() {
			return primitive;
		}// return CastTo<MassSpring>(primitive); }

	public:
		void SetStiff(float stiff) { CastTo<MassSpring>(primitive)->GetSimu()->SetStiff(stiff); };
		void SetFix(std::vector<unsigned>& fix) { fix_id = fix; };
		void SetLeftFix() {CastTo<MassSpring>(primitive)->GetSimu()->SetLeftFix();};
		void SetUpFix() { CastTo<MassSpring>(primitive)->GetSimu()->SetUpFix(); };
		void SetDownFix() { CastTo<MassSpring>(primitive)->GetSimu()->SetDownFix(); };
		void SetIndex(size_t index) { CastTo<MassSpring>(primitive)->GetSimu()->SetIndex(index); };
		const vecf3& GetFext() const { return CastTo<MassSpring>(primitive)->GetSimu()->GetFext(); }
		void SetFext(const pointf3& f) { CastTo<MassSpring>(primitive)->GetSimu()->SetFext(f); }
		void SetFextAll(const pointf3& f) { CastTo<MassSpring>(primitive)->GetSimu()->SetFext_all(f); }

	private:
		std::vector<unsigned> fix_id;
		float stiffness = 1000;
		Ptr<Primitive> primitive;
	};
}
