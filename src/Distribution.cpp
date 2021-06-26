#include <StatSim/Distribution.hpp>

#include <cassert>
#include <cmath>
#include <cstdio>
#include <memory>
#include <iomanip>
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
	double Interval::GetLength() const noexcept {
		return m_End - m_Begin;
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
		: m_Interval(interval), m_Center(center), m_Distance(distance) {
		assert(interval.GetLength() > 0);
	}

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
	double BinomialDistribution::GetProbability(double begin, double end) const {
		double result = 0;
		for (int i = static_cast<int>(std::ceil(begin)); i <= static_cast<int>(std::floor(end)); ++i) {
			static const auto combination = [](int n, int r) {
				if (n - r < r) {
					r = n - r;
				}

				long long result = 1;
				for (int i = 0; i < r; ++i) {
					result *= n - i;
				}
				for (int i = 0; i < r; ++i) {
					result /= i + 1;
				}
				return result;
			};

			result += combination(GetTryCount(), i) * std::pow(GetProbability(), i) * std::pow(1 - GetProbability(), GetTryCount() - i);
		}
		return result;
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
	double NormalDistribution::GetProbability(double begin, double end) const {
		static const auto cdf = [](double value) {
			return 0.5 * std::erfc(-value * std::sqrt(0.5));
		};
		return cdf((end - GetMean()) / GetStandardDeviation()) - cdf((begin - GetMean()) / GetStandardDeviation());
	}
}

namespace StatSim {
	PDFProgram PDFProgram::Load(const std::string_view& programPath) {
		PDFProgram result;
		result.m_ProgramPath = programPath;

		const int implementationLevel = static_cast<int>(result.GetValue("ILVL", 0));
		result.m_HasCDF = (implementationLevel >= 1);
		result.m_HasICDF = (implementationLevel >= 2);

		// TODO

		return result;
	}

	double PDFProgram::PDF(double x) const {
		return GetValue("PDF", x);
	}
	double PDFProgram::CDF(double x) const {
		if (m_HasCDF) return GetValue("CDF", x);

		// TODO
	}
	double PDFProgram::ICDF(double x) const {
		if (m_HasICDF) return GetValue("ICDF", x);

		// TODO
	}

	double PDFProgram::GetValue(const std::string_view& command, double x) const {
		std::ostringstream commandGenerator;
		commandGenerator << m_ProgramPath << ' ' << command << ' ' << std::fixed << x;

		FILE* const program = _popen(commandGenerator.str().c_str(), "r");
		assert(program);

		const std::unique_ptr<FILE, decltype(&_pclose)> programRaii(program, _pclose);
		std::string output;

		char buffer[128];
		while (fgets(buffer, sizeof(buffer), program)) {
			output += buffer;
		}
		return std::stod(output);
	}
}

namespace StatSim {
	void BinomialDistributionGenerator::SetParameter(const std::string& name, double value) {
		if (name == "TryCount") {
			m_TryCount = static_cast<int>(value);
		} else if (name == "Probability") {
			m_Probability = value;
		} else if (name == "Mean") {
			m_Mean = value;
		} else if (name == "Variance") {
			m_Variance = value;
		} else {
			assert(false);
		}
	}
	Distribution* BinomialDistributionGenerator::Generate() const {
		if (m_TryCount && m_Probability) {
			return new BinomialDistribution(*m_TryCount, *m_Probability);
		} else if (m_Mean && m_Variance) {
			const double q = *m_Variance / *m_Mean;
			const int n = static_cast<int>(*m_Mean / (1 - q));
			return new BinomialDistribution(n, 1 - q);
		} else {
			assert(false);
			return nullptr;
		}
	}
}

namespace StatSim {
	void NormalDistributionGenerator::SetParameter(const std::string& name, double value) {
		if (name == "Mean") {
			m_Mean = value;
		} else if (name == "Variance") {
			m_StandardDeviation = std::sqrt(value);
		} else if (name == "StandardDeviation") {
			m_StandardDeviation = value;
		} else {
			assert(false);
		}
	}
	Distribution* NormalDistributionGenerator::Generate() const {
		if (m_Mean && m_StandardDeviation) {
			return new NormalDistribution(*m_Mean, *m_StandardDeviation);
		} else {
			assert(false);
			return nullptr;
		}
	}
}