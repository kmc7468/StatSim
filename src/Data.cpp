#include <StatSim/Data.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iterator>
#include <numeric>
#include <stdexcept>
#include <random>

namespace StatSim {
	Data::Data(int size, Distribution* distribution)
		: m_Distribution(distribution) {
		assert(size >= 0);
		assert(m_Distribution != nullptr);

		std::generate_n(std::back_inserter(m_Data), size, [&]() {
			return distribution->Generate();
		});
		std::sort(m_Data.begin(), m_Data.end());
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

	double Data::GetProbability(double begin, double end) const noexcept {
		return std::count_if(m_Data.begin(), m_Data.end(), [&](double value) {
			return begin <= value && value <= end;
		}) / static_cast<double>(m_Data.size());
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

	Population::Population(std::vector<double>&& data, Distribution* distribution) noexcept
		: Data(std::move(data), distribution) {}

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
			for (int i = 0; i < size; ++i) {
				sample.push_back(distribution(g_Random));
			}
		} else {
			std::sample(begin(), end(), std::back_inserter(sample), size, g_Random);
		}

		std::sort(sample.begin(), sample.end());
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
	Population* Population::CreateSampleMeanPopulation(int size) const {
		std::vector<double> sampleMeans;
		for (const auto* sample : m_Samples.at(size)) {
			sampleMeans.push_back(sample->GetMean());
		}

		std::sort(sampleMeans.begin(), sampleMeans.end());
		return new Population(std::move(sampleMeans),
			new NormalDistribution(GetDistribution()->GetMean(), GetDistribution()->GetStandardDeviation() / std::sqrt(size)));
	}

	Population Population::Load(const std::string& path, DistributionGenerator* distributionGenerator) {
		std::ifstream file(path);
		if (!file) throw std::runtime_error("failed to open the file");

		std::vector<double> data{ std::istream_iterator<double>(file), std::istream_iterator<double>() };

		const double mean = std::accumulate(data.begin(), data.end(), 0.0) / data.size();
		const double variance = std::accumulate(data.begin(), data.end(), 0.0, [&](double sum, double x) {
			return sum + std::pow(x - mean, 2);
		}) / data.size();
		distributionGenerator->SetParameter("Mean", mean);
		distributionGenerator->SetParameter("Variance", variance);

		return { std::move(data), distributionGenerator->Generate() };
	}
	void Population::Save(const std::string& path) const {
		std::ofstream file(path);
		if (!file) throw std::runtime_error("failed to open the file");

		for (int i = 0; i < GetSize(); ++i) {
			file << (*this)[i] << '\n';
		}
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

	std::pair<Interval, double> Sample::GetConfidenceLevel(double k) const {
		assert(k > 0);

		static const auto cdf = [](double value) {
			return 0.5 * std::erfc(-value * std::sqrt(0.5));
		};

		const double m = GetMean();
		const double c = k * GetStandardDeviation() / std::sqrt(GetSize());
		return { { m - c, false, m + c, false }, cdf(k) - cdf(-k) };
	}
}