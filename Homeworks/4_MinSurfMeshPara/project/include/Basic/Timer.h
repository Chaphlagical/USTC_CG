#pragma once

#include <deque>
#include <iostream>

namespace Ubpa {
	class Timer
	{
	public:
		enum ENUM_STATE
		{
			ENUM_STATE_INIT,
			ENUM_STATE_RUNNING,
			ENUM_STATE_STOP,
		};

		Timer(bool start = false, size_t maxLogNum = 32);

		bool Start();
		bool Stop();

		// ���� ���� ��ʾ����
		double Log();

		void Reset();

		// ��ǰ���ڼ�¼��ʱ����λ�� 0
		// �¼�¼��ʱ�������ǰ��
		double GetLog(size_t i) const;

		ENUM_STATE GetState() const;
		double GetWholeTime() const;
		std::ostream& operator <<(std::ostream& os);
	private:
		double GetCurTime() const;

		ENUM_STATE state;
		std::deque<double> logs;
		const size_t maxLogNum;
		double lastLogTime;
		double wholeTime;
	};
}
