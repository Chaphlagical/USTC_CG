#pragma once

#include <Basic/Ptr.h>

namespace Ubpa {
	class HeapObj : public std::enable_shared_from_this<HeapObj> {
		// �� new �� Delete ��Ȩ�޽����� New ����
		template<typename ImplT, typename ...Args>
		friend const Ptr<ImplT> New(Args&& ... args);

	protected:
		// !!! �����ڹ��캯����ʹ�ã�����ʼ������ŵ� Init() ��
		template<typename T = HeapObj>
		const Ptr<T> This() { return CastTo<T>(shared_from_this()); }
		// !!! �����ڹ��캯����ʹ�ã�����ʼ������ŵ� Init() ��
		template<typename T = HeapObj>
		const PtrC<T> This() const { return CastTo<const T>(shared_from_this()); }
		// !!! �����ڹ��캯����ʹ�ã�����ʼ������ŵ� Init() ��
		template<typename T = HeapObj>
		const WPtr<T> WThis() noexcept { return CastTo<T>(weak_from_this()); }
		// !!! �����ڹ��캯����ʹ�ã�����ʼ������ŵ� Init() ��
		template<typename T = HeapObj>
		const WPtrC<T> WThis() const noexcept { return CastTo<const T>(weak_from_this()); }

	protected:
		// ���ڹ��캯���в���ʹ�� This(), WPtr()
		// ����ר���ṩ��һ�� Init_AfterGenPtr ����
		// �ú������� new �������� shared_ptr �����
		// ���Ըú����ڲ���ʹ�� This(), WPtr()
		virtual void Init_AfterGenPtr() {}

	protected:
		// ���� protected ���캯������������
		// ʹ���û�����ֱ����ջ�ϴ��� HeapObj ����
		// ���ǻ��ǿ�����ջ�ϴ��� ����
		// ��������ҲҪ protected ���캯������������

		HeapObj() = default;
		virtual ~HeapObj() = default;

	private:
		// ���� New ��ɾ��
		static void Delete(HeapObj* obj) {
			delete obj;
		}

	private:
		// private new �� delete
		// �����û����޷�ʹ�� new ��
		// ��������Ҳû�� new

		void* operator new(size_t size) {
			if (void* mem = malloc(size))
				return mem;
			else
				throw std::bad_alloc();
		}

		void operator delete(void* mem) noexcept {
			free(mem);
		}

	private:
		using std::enable_shared_from_this<HeapObj>::shared_from_this;
		using std::enable_shared_from_this<HeapObj>::weak_from_this;
	};

	// ���� ImplT �Ĺ��캯����Ȼ������ shared_ptr��Ȼ����� virtual �� Init_AfterGenPtr ����
	template<typename ImplT, typename ...Args>
	const Ptr<ImplT> New(Args&& ... args) {
		const auto pImplT = Ptr<ImplT>(new ImplT(std::forward<Args>(args)...), HeapObj::Delete);
		auto pHeapObj = static_cast<Ptr<HeapObj>>(pImplT);
		pHeapObj->Init_AfterGenPtr();
		return pImplT;
	}
}
