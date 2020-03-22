#pragma once

#ifdef USE_TINYXML2
#include <tinyxml2.h>
#endif // USE_TINYXML2


#include <Basic/Ptr.h>

#include <UGM/point.h>
#include <UGM/vec.h>
#include <UGM/normal.h>
#include <UGM/rgb.h>
#include <UGM/rgba.h>

#include <UTemplate/Func.h>

#include <string>
#include <map>
#include <functional>
#include <type_traits>

namespace Ubpa {
	class SObj;

	class SObjLoader {
	public:
		static Ptr<SObj> Load(const std::string& path);
#ifdef USE_TINYXML2
	private:
		using EleP = tinyxml2::XMLElement*;
		template <typename ...argType>
		using Func = std::function<void(argType...)>;
		using Key = std::string;
		using FuncMap = std::map<Key, Func<EleP>>;

	private:
		template<typename T>
		static const T To(const Key& key) {
			return To<T::N, typename T::T>(key).cast_to<T>();
		}

		template<size_t N, typename T>
		static const val<T, N> To(const Key& key) {
			assert(N == 2 || N == 3 || N == 4);
			T arr[N];
			size_t begin = 0;
			size_t end = -1;
			for (int i = 0; i < N - 1; i++) {
				end = key.find(' ', end + 1);
				if (end == std::string::npos) {
					printf("To<Val<%zd,%s>> ERROR: end == std::string::npos\n", N, typeid(T).name());
					return val<T, N>(static_cast<T>(0));
				}

				arr[i] = To<T>(key.substr(begin, end));
				begin = end;
			}
			if (begin == key.size()) {
				printf("To<Val<%zd,%s>> ERROR: begin == key.size()\n", N, typeid(T).name());
				return val<T, N>(static_cast<T>(0));
			}
			arr[N - 1] = To<T>(key.substr(begin, key.size()));

			val<T, N> rst;
			for (size_t i = 0; i < N; i++)
				rst[i] = arr[i];
			return rst;
		}

		// �� ele ��Ϊ ValImplType ����
		template<typename T>
		static T Load(EleP ele);

	private:
		// �� ele �ĺ��ӽ��� FuncMap ����
		static void LoadNode(EleP ele, const FuncMap& funcMap);

		// �� name == key �� ele ���������� func
		static void Reg_Func(FuncMap& funcMap, Key key, const Func<EleP>& func) {
			funcMap[key] = func;
		}

		template<typename LambdaExpr>
		static void Reg_Lambda(FuncMap& funcMap, Key key, LambdaExpr lambda) {
			funcMap[key] = lambda;
		}

		// �� name == key �� ele �� text �� T ����������func
		template<typename T>
		static void Reg_Text_Func(FuncMap& funcMap, Key key, const Func<const T&>& func) {
			Reg_Lambda(funcMap, key, [=](EleP ele) {
				const auto rst = To<T>(ele->GetText());
				func(rst);
				});
		}

		template<typename LambdaExpr>
		static void Reg_Text_Lambda(FuncMap& funcMap, Key key, LambdaExpr lambda) {
			using ValType = Front_t<typename FuncTraits<LambdaExpr>::ArgList>;
			const Func<const std::decay_t<ValType>&> func = lambda;
			Reg_Text_Func(funcMap, key, func);
		}

		// �� name == key �� ele �� text �� T ��ֵ�� val
		// �������� To<T>
		template<typename T>
		static void Reg_Text_val(FuncMap& funcMap, Key key, T& val) {
			Reg_Text_Lambda(funcMap, key, [&](const T& valFromText) {
				val = valFromText;
				});
		}

		// �� name == key �� ele �� text �� T ���������� obj �� setVal
		template<typename ValType, typename ObjType, typename RetType>
		static void Reg_Text_setVal(FuncMap& funcMap, Key key, Ptr<ObjType> obj, RetType(ObjType::* setVal)(ValType)) {
			Reg_Text_Lambda(funcMap, key, [=](const std::decay_t<ValType>& val) {
				((*obj).*setVal)(val);
				});
		}

		// �� name == key �� ele ���� Load<T>��������� func
		template<typename T>
		static void Reg_Load_Func(FuncMap& funcMap, Key key, const Func<const T&>& func) {
			Reg_Lambda(funcMap, key, [=](EleP ele) {
				const auto rst = Load<T>(ele);
				func(rst);
				});
		}

		template<typename LambdaExpr>
		static void Reg_Load_Lambda(FuncMap& funcMap, Key key, LambdaExpr lambda) {
			using ValType = Front_t<typename FuncTraits<LambdaExpr>::ArgList>;
			const Func<const std::decay_t<ValType>&> func = lambda;
			Reg_Load_Func(funcMap, key, func);
		}

		// �� name == key �� ele ���� Load<ImplT>��������Ƹ� ptrT
		template<typename ImplT, typename T>
		static void Reg_Load_val(FuncMap& funcMap, Key key, T& val) {
			Reg_Load_Lambda(funcMap, key, [&](const ImplT& valFormLoad) {
				val = valFormLoad;
				});
		}

		// �� ele ��Ϊ ValImpl ���룬������һ�����ú���
		template<typename ValImplType, typename ValBaseType, typename ObjType, typename RetType>
		static void Reg_Load_setVal(FuncMap& funcMap, Key key, Ptr<ObjType> obj, RetType(ObjType::* setVal)(Ptr<ValBaseType>)) {
			Reg_Load_Lambda(funcMap, key, [=](const Ptr<ValImplType>& ptr) {
				((*obj).*setVal)(ptr);
				});
		}
#endif
	};
}
