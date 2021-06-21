#pragma once

#include <numeric>
#include <optional>
#include <random>
#include <string>
#include <string_view>

namespace StatSim {
	class Interval final {
	public:
		static constexpr double Infinity = std::numeric_limits<double>::infinity();
		static const Interval Real;

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
		double GetLength() const noexcept;

		bool IsElement(double value) const noexcept;
	};
}

namespace StatSim {
	class RandomVariable final {
	private:
		Interval m_Interval;
		double m_Center, m_Distance;

	private:
		RandomVariable(Interval interval, double center, double distance) noexcept;

	public:
		RandomVariable(const RandomVariable& randomVariable) noexcept = default;
		~RandomVariable() = default;

	public:
		RandomVariable& operator=(const RandomVariable& randomVariable) = default;

	public:
		static RandomVariable Discrete(Interval interval, double center, double distance);
		static RandomVariable Continuous(Interval interval = Interval::Real);

	public:
		Interval GetInterval() const noexcept;
		double GetCenter() const noexcept;
		double GetDistance() const noexcept;
		bool IsDiscrete() const noexcept;
		bool IsContinuous() const noexcept;
	};
}

namespace StatSim {
	class Distribution {
	public:
		Distribution() noexcept = default;
		Distribution(const Distribution&) = delete;
		virtual ~Distribution() = default;

	public:
		Distribution& operator=(const Distribution&) = delete;

	public:
		virtual std::string GetName() const = 0;
		virtual std::string GetExpression() const = 0;
		virtual RandomVariable GetRandomVariable() const noexcept = 0;
		virtual Distribution* Copy() const = 0;

		virtual double GetMean() const = 0;
		virtual double GetVariance() const;
		virtual double GetStandardDeviation() const;

		virtual double Generate() = 0;
		virtual double GetProbability(double begin, double end) const = 0;
	};

	class BinomialDistribution final : public Distribution {
	private:
		std::binomial_distribution<> m_Distribution;

	public:
		BinomialDistribution(int tryCount, double probability) noexcept;
		BinomialDistribution(const BinomialDistribution&) = delete;
		virtual ~BinomialDistribution() override = default;

	public:
		BinomialDistribution& operator=(const BinomialDistribution&) = delete;

	public:
		virtual std::string GetName() const override;
		virtual std::string GetExpression() const override;
		virtual RandomVariable GetRandomVariable() const noexcept override;
		virtual Distribution* Copy() const override;

		virtual double GetMean() const override;
		virtual double GetVariance() const override;

		virtual double Generate() override;
		virtual double GetProbability(double begin, double end) const override;
		int GetTryCount() const noexcept;
		double GetProbability() const noexcept;
	};

	class NormalDistribution final : public Distribution {
	private:
		std::normal_distribution<> m_Distribution;

	public:
		NormalDistribution(double mean, double standardDeviation) noexcept;
		NormalDistribution(const BinomialDistribution&) = delete;
		virtual ~NormalDistribution() override = default;

	public:
		NormalDistribution& operator=(const NormalDistribution&) = delete;

	public:
		virtual std::string GetName() const override;
		virtual std::string GetExpression() const override;
		virtual RandomVariable GetRandomVariable() const noexcept override;
		virtual Distribution* Copy() const override;

		virtual double GetMean() const override;
		virtual double GetStandardDeviation() const override;

		virtual double Generate() override;
		virtual double GetProbability(double begin, double end) const override;
	};
}

namespace StatSim {
	class PDFProgram {
	private:
		std::string m_ProgramPath;
		bool m_HasCDF = false, m_HasICDF = false;

	private:
		PDFProgram() noexcept = default;

	public:
		PDFProgram(const PDFProgram& pdfProgram) = default;
		PDFProgram(PDFProgram&& pdfProgram) noexcept = default;

	public:
		PDFProgram& operator=(const PDFProgram& pdfProgram) = default;
		PDFProgram& operator=(PDFProgram&& pdfProgram) noexcept = default;

	public:
		static PDFProgram Load(const std::string_view& programPath);

	public:
		double PDF(double x) const;
		double CDF(double x) const;
		double ICDF(double x) const;

	private:
		double GetValue(const std::string_view& command, double x) const;
	};
}

namespace StatSim {
	class DistributionGenerator {
	public:
		DistributionGenerator() noexcept = default;
		DistributionGenerator(const DistributionGenerator&) = delete;
		virtual ~DistributionGenerator() = default;

	public:
		DistributionGenerator& operator=(const DistributionGenerator&) = delete;

	public:
		virtual void SetParameter(const std::string& name, double value) = 0;
		virtual Distribution* Generate() const = 0;
	};

	class BinomialDistributionGenerator final : public DistributionGenerator {
	private:
		std::optional<int> m_TryCount;
		std::optional<double> m_Probability;
		std::optional<double> m_Mean;
		std::optional<double> m_Variance;

	public:
		BinomialDistributionGenerator() noexcept = default;
		BinomialDistributionGenerator(const BinomialDistributionGenerator&) = delete;
		virtual ~BinomialDistributionGenerator() override = default;

	public:
		BinomialDistributionGenerator& operator=(const BinomialDistributionGenerator&) = delete;

	public:
		virtual void SetParameter(const std::string& name, double value) override;
		virtual Distribution* Generate() const override;
	};

	class NormalDistributionGenerator final : public DistributionGenerator {
	private:
		std::optional<double> m_Mean;
		std::optional<double> m_StandardDeviation;

	public:
		NormalDistributionGenerator() noexcept = default;
		NormalDistributionGenerator(const NormalDistributionGenerator&) = delete;
		virtual ~NormalDistributionGenerator() override = default;

	public:
		NormalDistributionGenerator& operator=(const NormalDistributionGenerator&) = delete;

	public:
		virtual void SetParameter(const std::string& name, double value) override;
		virtual Distribution* Generate() const override;
	};
}