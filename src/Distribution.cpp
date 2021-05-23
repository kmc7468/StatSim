#include <StatSim/Distribution.hpp>

#include <cassert>
#include <cmath>

namespace StatSim {
	Interval::Interval(double begin, double end) noexcept
		: Interval(begin, true, end, true) {}
	Interval::Interval(double begin, bool isBeginOpen, double end, bool isEndOpen) noexcept
		: m_Begin(begin), m_End(end), m_IsBeginOpen(isBeginOpen), m_IsEndOpen(isEndOpen) {
		assert(begin < end);

		if (begin == -Infinity) {
			m_IsBeginOpen = true;
		}
		if (end == Infinity) {
			m_IsEndOpen = true;
		}
	}

	double Interval::GetBegin() const noexcept {
		return m_Begin;
	}
	double Interval::GetEnd() const noexcept {
		return m_End;
	}
	bool Interval::IsBeginOpen() const noexcept {
		return m_IsBeginOpen;
	}
	bool Interval::IsEndOpen() const noexcept {
		return m_IsEndOpen;
	}
}

namespace StatSim {
	double Distribution::GetVariance() const {
		return std::pow(GetStandardDeviation(), 2);
	}
	double Distribution::GetStandardDeviation() const {
		return std::sqrt(GetVariance());
	}
}