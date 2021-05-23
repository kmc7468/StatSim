#pragma once

#include <StatSim/Distribution.hpp>

#include <map>
#include <string>
#include <vector>

namespace StatSim {
	class Data {
	private:
		std::vector<double> m_Data;
		Distribution* m_Distribution;

	protected:
		Data(int size, Distribution* distribution);
		Data(std::vector<double>&& data, Distribution* distribution) noexcept;

	public:
		Data(const Data&) = delete;
		Data(Data&& data) noexcept;
		virtual ~Data();

	public:
		Data& operator=(const Data&) = delete;
		Data& operator=(Data&& data) noexcept;
		double operator[](int index) const noexcept;

	public:
		virtual std::string GetName() const = 0;
		const Distribution* GetDistribution() const noexcept;
		Distribution* GetDistribution() noexcept;

		int GetSize() const noexcept;
		virtual double GetMean() const;
		virtual double GetVariance() const;
		virtual double GetStandardDeviation() const;

	protected:
		std::vector<double>::const_iterator begin() const;
		std::vector<double>::const_iterator end() const;

		double GetSum() const noexcept;
		double GetDeviationSum(double mean) const noexcept;
	};

	class Sample;

	class Population final : public Data {
	private:
		std::map<int, std::vector<StatSim::Sample*>> m_Samples;
		int m_SampleCount = 0;

	private:
		Population(std::vector<double>&& data, Distribution* distribution) noexcept;

	public:
		Population(int size, Distribution* distribution);
		Population(const Population&) = delete;
		Population(Population&& population) noexcept = default;
		virtual ~Population() override;

	public:
		Population& operator=(const Population&) = delete;
		Population& operator=(Population&& population) noexcept = default;

	public:
		virtual std::string GetName() const override;

		StatSim::Sample* Sample(int size, bool enableReplacement);
		const StatSim::Sample* GetSample(int index) const;
		StatSim::Sample* GetSample(int index);
		std::map<int, std::vector<const StatSim::Sample*>> GetSamples() const;
		std::map<int, std::vector<StatSim::Sample*>> GetSamples();
		int GetSampleCount() const noexcept;
		Population* CreateSampleMeanPopulation(int size) const;
	};

	class Sample final : public Data {
	private:
		Population* m_Population = nullptr;
		int m_Index;

	public:
		Sample(Population* population, int index, std::vector<double>&& data, Distribution* distribution) noexcept;
		Sample(const Sample&) = delete;
		Sample(Sample&& sample) noexcept = default;
		virtual ~Sample() override = default;

	public:
		Sample& operator=(const Sample&) = delete;
		Sample& operator=(Sample&& sample) noexcept = default;

	public:
		virtual std::string GetName() const override;
		const Population* GetPopulation() const noexcept;
		Population* GetPopulation() noexcept;
		int GetIndex() const noexcept;

		virtual double GetVariance() const override;
	};
}