#pragma once

#include <numeric>
#include <string>

namespace StatSim {
	class Interval final {
	public:
		static constexpr double Infinity = std::numeric_limits<double>::infinity();

	private:
		double m_Begin, m_End;
		bool m_IsBeginOpen, m_IsEndOpen;

	public:
		Interval(double begin, double end) noexcept;
		Interval(double begin, bool isBeginOpen, double end, bool isEndOpen) noexcept;
		Interval(const Interval& interval) noexcept = default;
		~Interval() = default;

	public:
		Interval& operator=(const Interval& interval) noexcept = default;

	public:
		double GetBegin() const noexcept;
		double GetEnd() const noexcept;
		bool IsBeginOpen() const noexcept;
		bool IsEndOpen() const noexcept;
	};
}

namespace StatSim {
	class RandomVariable final {
	private:
		Interval m_Interval;
		double m_Distance;

	public:
		RandomVariable(Interval interval, double distance) noexcept;
		RandomVariable(const RandomVariable& randomVariable) noexcept = default;
		~RandomVariable() = default;

	public:
		RandomVariable& operator=(const RandomVariable& randomVariable) = default;

	public:
		Interval GetInterval() const noexcept;
		double GetDistance() const noexcept;
		bool IsDiscrete() const noexcept;
		bool IsContinuous() const noexcept;
	};
}

namespace StatSim {
	class Distribution {
	public:
		Distribution() = delete;
		Distribution(const Distribution&) = delete;
		virtual ~Distribution() = default;

	public:
		Distribution& operator=(const Distribution&) = delete;

	public:
		virtual std::string GetName() const = 0;
		virtual std::string GetExpression() const = 0;

		virtual double GetMean() const = 0;
		virtual double GetVariance() const;
		virtual double GetStandardDeviation() const;
	};
}