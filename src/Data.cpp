#include <StatSim/Data.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iterator>
#include <numeric>
#include <random>
#include <utility>

namespace StatSim {
	Data::Data(int size, Distribution* distribution)
		: m_Distribution(distribution) {
		assert(size >= 0);
		assert(m_Distribution != nullptr);

		std::generate_n(std::back_inserter(m_Data), size, [&]() {
			return distribution->Generate();
		});
	}
	Data::Data(std::vector<double>&& data, Distribution* distribution) noexcept
		: m_Data(std::move(data)), m_Distribution(distribution) {}

	Data::Data(Data&& data) noexcept
		: m_Data(std::move(data.m_Data)), m_Distribution(data.m_Distribution) {
		data.m_Distribution = nullptr;
	}
	Data::~Data() {
		delete m_Distribution;
	}

	Data& Data::operator=(Data&& data) noexcept {
		delete m_Distribution;

		m_Data = std::move(data.m_Data);
		m_Distribution = data.m_Distribution;

		data.m_Distribution = nullptr;
		return *this;
	}

	double Data::operator[](int index) const noexcept {
		return m_Data[index];
	}

	const Distribution* Data::GetDistribution() const noexcept {
		return m_Distribution;
	}
	Distribution* Data::GetDistribution() noexcept {
		return m_Distribution;
	}

	int Data::GetSize() const noexcept {
		return static_cast<int>(m_Data.size());
	}
	double Data::GetMean() const {
		return GetSum() / GetSize();
	}
	double Data::GetVariance() const {
		return GetDeviationSum(GetMean()) / GetSize();
	}
	double Data::GetStandardDeviation() const {
		return std::sqrt(GetVariance());
	}

	std::vector<double>::const_iterator Data::begin() const {
		return m_Data.begin();
	}
	std::vector<double>::const_iterator Data::end() const {
		return m_Data.end();
	}

	double Data::GetSum() const noexcept {
		return std::accumulate(m_Data.begin(), m_Data.end(), 0.0);
	}
	double Data::GetDeviationSum(double mean) const noexcept {
		return std::accumulate(m_Data.begin(), m_Data.end(), 0.0, [&](double sum, double x) {
			return sum + std::pow(x - mean, 2);
		});
	}
}

namespace StatSim {
	static std::mt19937 g_Random(std::random_device{}());

	Population::Population(int size, Distribution* distribution)
		: Data(size, distribution) {}
	Population::~Population() {
		for (auto& [size, samples] : m_Samples) {
			for (auto* sample : samples) {
				delete sample;
			}
		}
	}

	std::string Population::GetName() const {
		return "모집단";
	}

	StatSim::Sample* Population::Sample(int size, bool enableReplacement) {
		std::vector<double> sample;
		if (enableReplacement) {
			std::uniform_int_distribution<> distribution(0, GetSize() - 1);
			std::for_each_n(std::back_inserter(sample), size, [&](auto& x) {
				*x = (*this)[distribution(g_Random)];
			});
		} else {
			std::sample(begin(), end(), std::back_inserter(sample), size, g_Random);
		}
		return m_Samples[size].emplace_back(new StatSim::Sample(this, m_SampleCount++, std::move(sample), GetDistribution()->Copy()));
	}
	const StatSim::Sample* Population::GetSample(int index) const {
		return const_cast<Population*>(this)->GetSample(index);
	}
	StatSim::Sample* Population::GetSample(int index) {
		for (auto& [size, samples] : m_Samples) {
			const auto iter = std::find_if(samples.begin(), samples.end(), [&](const auto* sample) {
				return sample->GetIndex() == index;
			});
			if (iter != samples.end()) return *iter;
		}
		return nullptr;
	}
	std::map<int, std::vector<const StatSim::Sample*>> Population::GetSamples() const {
		std::map<int, std::vector<const StatSim::Sample*>> result;
		for (const auto& [size, samples] : m_Samples) {
			result[size] = { samples.begin(), samples.end() };
		}
		return result;
	}
	std::map<int, std::vector<StatSim::Sample*>> Population::GetSamples() {
		return m_Samples;
	}
	int Population::GetSampleCount() const noexcept {
		return m_SampleCount;
	}
}

namespace StatSim {
	Sample::Sample(Population* population, int index, std::vector<double>&& data, Distribution* distribution) noexcept
		: Data(std::move(data), distribution), m_Population(population), m_Index(index) {
		assert(index >= 0);
	}

	std::string Sample::GetName() const {
		return "표본 #" + std::to_string(m_Index);
	}
	const Population* Sample::GetPopulation() const noexcept {
		return m_Population;
	}
	Population* Sample::GetPopulation() noexcept {
		return m_Population;
	}
	int Sample::GetIndex() const noexcept {
		return m_Index;
	}

	double Sample::GetVariance() const {
		return GetDeviationSum(GetMean()) / (GetSize() - 1);
	}
}