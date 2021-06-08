#include <StatSim/Distribution.hpp>

#include <cassert>
#include <cmath>
#include <sstream>

namespace StatSim {
	const Interval Interval::Real(-Interval::Infinity, Interval::Infinity);

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

	bool Interval::IsElement(double value) const noexcept {
		const double differenceFromBegin = value - m_Begin;
		const double differenceFromEnd = m_End - value;
		return (m_IsBeginOpen ? differenceFromBegin > 0 : differenceFromBegin >= 0) &&
			(m_IsEndOpen ? differenceFromEnd > 0 : differenceFromEnd >= 0);
	}
}

namespace StatSim {
	RandomVariable::RandomVariable(Interval interval, double center, double distance) noexcept
		: m_Interval(interval), m_Center(center), m_Distance(distance) {}

	RandomVariable RandomVariable::Discrete(Interval interval, double center, double distance) {
		assert(interval.IsElement(center));
		assert(distance > 0);

		return { interval, center, distance };
	}
	RandomVariable RandomVariable::Continuous(Interval interval) {
		return { interval, 0, 0 };
	}

	Interval RandomVariable::GetInterval() const noexcept {
		return m_Interval;
	}
	double RandomVariable::GetCenter() const noexcept {
		return m_Center;
	}
	double RandomVariable::GetDistance() const noexcept {
		return m_Distance;
	}
	bool RandomVariable::IsDiscrete() const noexcept {
		return m_Distance != 0;
	}
	bool RandomVariable::IsContinuous() const noexcept {
		return m_Distance == 0;
	}
}

namespace StatSim {
	static std::mt19937 g_Random(std::random_device{}());

	double Distribution::GetVariance() const {
		return std::pow(GetStandardDeviation(), 2);
	}
	double Distribution::GetStandardDeviation() const {
		return std::sqrt(GetVariance());
	}
}

namespace StatSim {
	BinomialDistribution::BinomialDistribution(int tryCount, double probability) noexcept
		: m_Distribution(tryCount, probability) {}

	std::string BinomialDistribution::GetName() const {
		return "이항분포";
	}
	std::string BinomialDistribution::GetExpression() const {
		std::ostringstream oss;
		oss << "B(" << GetTryCount() << ", " << GetProbability() << ')';
		return oss.str();
	}
	RandomVariable BinomialDistribution::GetRandomVariable() const noexcept {
		return RandomVariable::Discrete({ 0, (double)GetTryCount() }, 0, 1);
	}
	Distribution* BinomialDistribution::Copy() const {
		return new BinomialDistribution(GetTryCount(), GetProbability());
	}

	double BinomialDistribution::GetMean() const {
		return GetTryCount() * GetProbability();
	}
	double BinomialDistribution::GetVariance() const {
		return GetTryCount() * GetProbability() * (1 - GetProbability());
	}

	double BinomialDistribution::Generate() {
		return m_Distribution(g_Random);
	}
	int BinomialDistribution::GetTryCount() const noexcept {
		return m_Distribution.t();
	}
	double BinomialDistribution::GetProbability() const noexcept {
		return m_Distribution.p();
	}
}

namespace StatSim {
	NormalDistribution::NormalDistribution(double mean, double standardDeviation) noexcept
		: m_Distribution(mean, standardDeviation) {}

	std::string NormalDistribution::GetName() const {
		return "정규분포";
	}
	std::string NormalDistribution::GetExpression() const {
		std::ostringstream oss;
		oss << "N(" << GetMean() << ", " << GetStandardDeviation() << "²)";
		return oss.str();
	}
	RandomVariable NormalDistribution::GetRandomVariable() const noexcept {
		return RandomVariable::Continuous();
	}
	Distribution* NormalDistribution::Copy() const {
		return new NormalDistribution(GetMean(), GetStandardDeviation());
	}

	double NormalDistribution::GetMean() const {
		return m_Distribution.mean();
	}
	double NormalDistribution::GetStandardDeviation() const {
		return m_Distribution.stddev();
	}

	double NormalDistribution::Generate() {
		return m_Distribution(g_Random);
	}
}