#pragma once

#include <Engine/Scene/Component.h>

namespace Ubpa {
	class Primitive;

	class CmptGeometry : public Component {
	public:
		CmptGeometry(Ptr<SObj> sobj = nullptr, Ptr<Primitive> primitive = nullptr)
			: Component(sobj), primitive(primitive) { }

	public:
		static const Ptr<CmptGeometry> New(Ptr<SObj> sobj, Ptr<Primitive> primitive) {
			return Ubpa::New<CmptGeometry>(sobj, primitive);
		}

	protected:
		virtual ~CmptGeometry() = default;

	public:
		Ptr<Primitive> primitive;
	};
}
