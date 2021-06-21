#include <StatSim/Simulator.hpp>

#include <iostream>
#include <string_view>

template<typename T>
T ReadInput(const char* name);
template<>
std::string ReadInput<std::string>(const char* name);
int ReadAction(const char* name, int max, const std::string_view& actions);

int main() {
	StatSim::Simulator simulator;
	std::cout << std::fixed;

	// 1. 모집단 생성
	// 1-1. 모집단 생성 방식
	const int populationGenerationMethod = ReadAction("모집단 생성 방식", 2, "1. 확률분포에 따라 생성하기\n2. 파일에서 불러오기");
	std::string populationPath;
	if (populationGenerationMethod == 2) {
		std::cin.ignore();
		populationPath = ReadInput<std::string>("파일 경로");
	}

	// 1-2. 모집단 확률분포
	StatSim::DistributionGenerator* distributionGenerator = nullptr;
	switch (ReadAction("확률변수 유형", 2, "1. 이산확률변수\n2. 연속확률변수")) {
	case 1:
		switch (ReadAction("확률분포", 1, "1. 이항분포")) {
		case 1: {
			distributionGenerator = new StatSim::BinomialDistributionGenerator();
			if (populationGenerationMethod == 1) {
				distributionGenerator->SetParameter("TryCount", ReadInput<int>("시도 횟수"));
				distributionGenerator->SetParameter("Probability", ReadInput<double>("사건의 확률"));
			}
			break;
		}
		}
		break;

	case 2:
		switch (ReadAction("확률분포", 1, "1. 정규분포")) {
		case 1: {
			distributionGenerator = new StatSim::NormalDistributionGenerator();
			if (populationGenerationMethod == 1) {
				distributionGenerator->SetParameter("Mean", ReadInput<int>("모평균"));
				distributionGenerator->SetParameter("StandardDeviation", ReadInput<double>("모표준편차"));
			}
			break;
		}
		}
		break;
	}

	// 1-3. 모집단 생성
	if (populationGenerationMethod == 1) {
		const int populationSize = ReadInput<int>("모집단의 크기");
		simulator.GeneratePopulation(populationSize, distributionGenerator->Generate());
	} else {
		simulator.LoadPopulation(populationPath, distributionGenerator);
	}
	delete distributionGenerator;

	// 2. 동작
	while (true) {
		StatSim::Data* const data = simulator.GetSelectedData();
		StatSim::Distribution* const distribution = data->GetDistribution();

		std::cout << "----------\n동작 대상: " << data->GetName() << '\n';
		if (simulator.IsPopulationSelected()) {
			switch (ReadAction("동작", 8, "1. 출력\n2. 확률분포\n3. 전수조사\n4. 표본 목록\n5. 표본 추출\n6. 표본 선택\n7. 표본평균의 분포\n8. 표본평균의 분포 저장")) {
			case 1:
				simulator.PrintSelectedData();
				std::cout << '\n';
				break;

			case 2:
				std::cout << distribution->GetExpression() << '\n';
				break;

			case 3: {
				const double mathMean = distribution->GetMean(), statMean = data->GetMean(), errMean = statMean - mathMean;
				std::cout << "수학적 모평균: " << mathMean << "\n통계적 모평균: " << statMean << "(오차 " << errMean << ")\n";

				const double mathVariance = distribution->GetVariance(), statVariance = data->GetVariance(), errVariance = statVariance - mathVariance;
				std::cout << "수학적 모분산: " << mathVariance << "\n통계적 모분산: " << statVariance << "(오차 " << errVariance << ")\n";

				const double mathStandardDeviation = distribution->GetStandardDeviation(), statStandardDeviation = data->GetStandardDeviation(),
					errStandardDeviation = statStandardDeviation - mathStandardDeviation;
				std::cout << "수학적 모표준편차: " << mathStandardDeviation << "\n통계적 모표준편차: " << statStandardDeviation << "(오차 " << errStandardDeviation << ")\n";
				break;
			}

			case 4: {
				StatSim::Population* const population = static_cast<StatSim::Population*>(data);
				const auto samples = population->GetSamples();
				for (const auto& [size, samples] : samples) {
					std::cout << "n=" << size << "인 표본\n";

					StatSim::Sample* firstSample = nullptr;
					for (int i = 0; i < static_cast<int>(samples.size()); ++i) {
						if (firstSample == nullptr) {
							firstSample = samples[i];
						}

						while (i + 1 < static_cast<int>(samples.size()) && samples[i + 1]->GetIndex() == samples[i]->GetIndex() + 1) {
							++i;
						}

						if (firstSample == samples[i]) {
							std::cout << "- " << firstSample->GetName() << '\n';
						} else {
							std::cout << "- " << firstSample->GetName() << " ~ " << samples[i]->GetName() << '\n';
						}
						firstSample = nullptr;
					}
				}
				break;
			}

			case 5: {
				const int size = ReadInput<int>("표본의 크기");
				if (size < 1 || size > data->GetSize()) {
					std::cout << "올바르지 않은 크기입니다.\n";
					break;
				}
				const int enableReplacement = ReadAction("추출 방법", 2, "1. 비복원 추출\n2. 복원 추출") - 1;
				const int loop = ReadInput<int>("추출 횟수");
				if (loop < 1) {
					std::cout << "올바르지 않은 횟수입니다.\n";
					break;
				}

				StatSim::Population* const population = static_cast<StatSim::Population*>(data);

				StatSim::Sample* firstSample = nullptr;
				StatSim::Sample* lastSample = nullptr;
				for (int i = 0; i < loop; ++i) {
					StatSim::Sample* const sample = population->Sample(size, enableReplacement);
					if (i == 0) {
						lastSample = firstSample = sample;
					} else if (i + 1 == loop) {
						lastSample = sample;
					}
				}

				if (firstSample == lastSample) {
					std::cout << firstSample->GetName() << "이 생성되었습니다.\n";
				} else {
					std::cout << firstSample->GetName() << " ~ " << lastSample->GetName() << "이 생성되었습니다.\n";
				}
				break;
			}

			case 6: {
				StatSim::Population* const population = static_cast<StatSim::Population*>(data);

				const int index = ReadInput<int>("표본의 번호");
				if (index < 0 || index > population->GetSampleCount()) {
					std::cout << "올바르지 않은 번호입니다.\n";
					break;
				}

				simulator.SelectSample(index);
				break;
			}

			case 7: {
				const int size = ReadInput<int>("표본의 크기");
				if (size < 1 || size > data->GetSize()) {
					std::cout << "올바르지 않은 크기입니다.\n";
					break;
				}

				StatSim::Population* const population = static_cast<StatSim::Population*>(data);
				const auto samples = population->GetSamples();
				if (samples.find(size) == samples.end()) {
					std::cout << "해당 크기의 표본이 존재하지 않습니다.\n";
					break;
				}

				StatSim::Population* const sampleMeans = population->CreateSampleMeanPopulation(size);
				StatSim::Distribution* const sampleMeansDistribution = sampleMeans->GetDistribution();

				const double mathMean = sampleMeansDistribution->GetMean(), statMean = sampleMeans->GetMean(), errMean = statMean - mathMean;
				std::cout << "표본평균의 수학적 평균: " << mathMean << "\n표본평균의 통계적 평균: " << statMean << "(오차 " << errMean << ")\n";

				const double mathVariance = sampleMeansDistribution->GetVariance(), statVariance = sampleMeans->GetVariance(),
					errVariance = statVariance - mathVariance;
				std::cout << "표본평균의 수학적 분산: " << mathVariance << "\n표본평균의 통계적 분산: " << statVariance << "(오차 " << errVariance << ")\n";

				const double mathStandardDeviation = sampleMeansDistribution->GetStandardDeviation(), statStandardDeviation = sampleMeans->GetStandardDeviation(),
					errStandardDeviation = statStandardDeviation - mathStandardDeviation;
				std::cout << "표본평균의 수학적 표준편차: " << mathStandardDeviation << "\n표본평균의 통계적 표준편차: " << statStandardDeviation << "(오차 " << errStandardDeviation << ")\n";
				break;
			}

			case 8: {
				const int size = ReadInput<int>("표본의 크기");
				if (size < 1 || size > data->GetSize()) {
					std::cout << "올바르지 않은 크기입니다.\n";
					break;
				}

				StatSim::Population* const population = static_cast<StatSim::Population*>(data);
				const auto samples = population->GetSamples();
				if (samples.find(size) == samples.end()) {
					std::cout << "해당 크기의 표본이 존재하지 않습니다.\n";
					break;
				}

				StatSim::Population* const sampleMeans = population->CreateSampleMeanPopulation(size);

				std::cin.ignore();
				const std::string path = ReadInput<std::string>("파일의 경로");
				sampleMeans->Save(path);
				break;
			}
			}
		} else {
			switch (ReadAction("동작", 5, "1. 출력\n2. 확률분포\n3. 표본조사\n4. 모평균 추정\n5. 모집단 선택")) {
			case 1:
				simulator.PrintSelectedData();
				std::cout << '\n';
				break;

			case 2:
				std::cout << distribution->GetExpression() << '\n';
				break;

			case 3: {
				StatSim::Sample* const sample = static_cast<StatSim::Sample*>(data);
				StatSim::Population* const population = sample->GetPopulation();

				const double statPopMean = population->GetMean(), statMean = data->GetMean(), errMean = statMean - statPopMean;
				std::cout << "통계적 모평균: " << statPopMean << "\n통계적 표본평균: " << statMean << "(오차 " << errMean << ")\n";

				const double statPopVariance = population->GetVariance(), statVariance = data->GetVariance(), errVariance = statVariance - statPopVariance;
				std::cout << "통계적 모분산: " << statPopVariance << "\n통계적 표본분산: " << statVariance << "(오차 " << errVariance << ")\n";

				const double statPopStandardDeviation = population->GetStandardDeviation(), statStandardDeviation = data->GetStandardDeviation(),
					errStandardDeviation = statStandardDeviation - statPopStandardDeviation;
				std::cout << "통계적 모표준편차: " << statPopStandardDeviation << "\n통계적 표본표준편차: " << statStandardDeviation << "(오차 " << errStandardDeviation << ")\n";
				break;
			}

			case 4: {
				StatSim::Sample* const sample = static_cast<StatSim::Sample*>(data);
				StatSim::Population* const population = sample->GetPopulation();

				const double statPopMean = population->GetMean(), statMean = data->GetMean(), errMean = statPopMean - statMean;
				const double statStandardDeviation = data->GetStandardDeviation();
				const double reliMean = 0.5 * std::erfc(-(errMean / statStandardDeviation) * std::sqrt(0.5)) -
					0.5 * std::erfc(-(-errMean / statStandardDeviation) * std::sqrt(0.5));
				std::cout << "통계적 모평균: " << statPopMean << "\n통계적 표본평균: " << statMean << "\n신뢰도 " << std::abs(reliMean * 100) << "% 구간 내에 통계적 모평균이 존재합니다.\n";
				break;
			}

			case 5:
				simulator.SelectPopulation();
				break;
			}
		}
	}

	return 0;
}

template<typename T>
T ReadInput(const char* name) {
	std::cout << name << " >>> ";
	T result;
	return std::cin >> result, result;
}
template<>
std::string ReadInput<std::string>(const char* name) {
	std::cout << name << " >>> ";

	std::string result;
	std::getline(std::cin, result);
	return result;
}
int ReadAction(const char* name, int max, const std::string_view& actions) {
	std::cout << name << '\n' << actions;

	do {
		int result;
		std::cout << "\n>>> ";
		std::cin >> result;

		if (result < 1 || result > max) {
			std::cout << "알 수 없는 동작입니다.";
		} else return result;
	} while (true);
}