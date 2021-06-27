#include <StatSim/Simulator.hpp>

#include <StatSim/Math.hpp>

#include <ios>
#include <iostream>
#include <string>
#include <string_view>

template<typename T>
T ReadInput(const char* name);
template<>
std::string ReadInput<std::string>(const char* name);
int ReadAction(const char* name, int max, const std::string_view& actions);

int main() {
	StatSim::Simulator simulator;
	std::cout << std::fixed;

	// 1. ������ ����
	// 1-1. ������ ���� ���
	const int populationGenerationMethod = ReadAction("������ ���� ���", 2, "1. Ȯ�������� ���� �����ϱ�\n2. ���Ͽ��� �ҷ�����");
	std::string populationPath;
	if (populationGenerationMethod == 2) {
		std::cin.ignore();
		populationPath = ReadInput<std::string>("���� ���");
	}

	// 1-2. ������ Ȯ������
	StatSim::DistributionGenerator* distributionGenerator = nullptr;
	switch (ReadAction("Ȯ������ ����", 2, "1. �̻�Ȯ������\n2. ����Ȯ������")) {
	case 1:
		switch (ReadAction("Ȯ������", 1, "1. ���׺���")) {
		case 1: {
			distributionGenerator = new StatSim::BinomialDistributionGenerator();
			if (populationGenerationMethod == 1) {
				distributionGenerator->SetParameter("TryCount", ReadInput<int>("�õ� Ƚ��"));
				distributionGenerator->SetParameter("Probability", ReadInput<double>("����� Ȯ��"));
			}
			break;
		}
		}
		break;

	case 2:
		switch (ReadAction("Ȯ������", 1, "1. ���Ժ���")) {
		case 1: {
			distributionGenerator = new StatSim::NormalDistributionGenerator();
			if (populationGenerationMethod == 1) {
				distributionGenerator->SetParameter("Mean", ReadInput<int>("�����"));
				distributionGenerator->SetParameter("StandardDeviation", ReadInput<double>("��ǥ������"));
			}
			break;
		}
		}
		break;
	}

	// 1-3. ������ ����
	if (populationGenerationMethod == 1) {
		const int populationSize = ReadInput<int>("�������� ũ��");
		simulator.GeneratePopulation(populationSize, distributionGenerator->Generate());
	} else {
		simulator.LoadPopulation(populationPath, distributionGenerator);
	}
	delete distributionGenerator;

	// 2. ����
	while (true) {
		StatSim::Data* const data = simulator.GetSelectedData();
		StatSim::Distribution* const distribution = data->GetDistribution();

		std::cout << "----------\n���� ���: " << data->GetName() << '\n';
		if (simulator.IsPopulationSelected()) {
			switch (ReadAction("����", 10, "1. ���\n2. Ȯ������\n3. ��������\n4. Ȯ�� ���\n5. ǥ�� ���\n"
				"6. ǥ�� ����\n7. ǥ�� ����\n8. ǥ������� ����\n9. ǥ������� ���� ����\n10. ����� ���� ���� Ȯ�� ���")) {
			case 1:
				simulator.PrintSelectedData();
				std::cout << '\n';
				break;

			case 2:
				std::cout << distribution->GetExpression() << '\n';
				break;

			case 3: {
				const double mathMean = distribution->GetMean(), statMean = data->GetMean(), errMean = statMean - mathMean;
				std::cout << "������ �����: " << mathMean << "\n����� �����: " << statMean << "(���� " << errMean << ")\n";

				const double mathVariance = distribution->GetVariance(), statVariance = data->GetVariance(), errVariance = statVariance - mathVariance;
				std::cout << "������ ��л�: " << mathVariance << "\n����� ��л�: " << statVariance << "(���� " << errVariance << ")\n";

				const double mathStandardDeviation = distribution->GetStandardDeviation(), statStandardDeviation = data->GetStandardDeviation(),
					errStandardDeviation = statStandardDeviation - mathStandardDeviation;
				std::cout << "������ ��ǥ������: " << mathStandardDeviation << "\n����� ��ǥ������: " << statStandardDeviation << "(���� " << errStandardDeviation << ")\n";
				break;
			}

			case 4: {
				std::cout << "P(a<=X<=b)�� ���� ����մϴ�.\n";
				const double a = ReadInput<double>("a�� ��");
				const double b = ReadInput<double>("b�� ��");

				const double mathProbability = distribution->GetProbability(a, b), statProbability = data->GetProbability(a, b),
					errProbability = statProbability - mathProbability;
				std::cout << "������ Ȯ��: " << mathProbability << "\n����� Ȯ��: " << statProbability << "(���� " << errProbability << ")\n";
				break;
			}

			case 5: {
				StatSim::Population* const population = static_cast<StatSim::Population*>(data);
				const auto samples = population->GetSamples();
				for (const auto& [size, samples] : samples) {
					std::cout << "n=" << size << "�� ǥ��\n";

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

			case 6: {
				const int size = ReadInput<int>("ǥ���� ũ��");
				if (size < 1 || size > data->GetSize()) {
					std::cout << "�ùٸ��� ���� ũ���Դϴ�.\n";
					break;
				}
				const int enableReplacement = ReadAction("���� ���", 2, "1. �񺹿� ����\n2. ���� ����") - 1;
				const int loop = ReadInput<int>("���� Ƚ��");
				if (loop < 1) {
					std::cout << "�ùٸ��� ���� Ƚ���Դϴ�.\n";
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
					std::cout << firstSample->GetName() << "�� �����Ǿ����ϴ�.\n";
				} else {
					std::cout << firstSample->GetName() << " ~ " << lastSample->GetName() << "�� �����Ǿ����ϴ�.\n";
				}
				break;
			}

			case 7: {
				StatSim::Population* const population = static_cast<StatSim::Population*>(data);

				const int index = ReadInput<int>("ǥ���� ��ȣ");
				if (index < 0 || index > population->GetSampleCount()) {
					std::cout << "�ùٸ��� ���� ��ȣ�Դϴ�.\n";
					break;
				}

				simulator.SelectSample(index);
				break;
			}

			case 8: {
				const int size = ReadInput<int>("ǥ���� ũ��");
				if (size < 1 || size > data->GetSize()) {
					std::cout << "�ùٸ��� ���� ũ���Դϴ�.\n";
					break;
				}

				StatSim::Population* const population = static_cast<StatSim::Population*>(data);
				const auto samples = population->GetSamples();
				if (samples.find(size) == samples.end()) {
					std::cout << "�ش� ũ���� ǥ���� �������� �ʽ��ϴ�.\n";
					break;
				}

				StatSim::Population* const sampleMeans = population->CreateSampleMeanPopulation(size);
				StatSim::Distribution* const sampleMeansDistribution = sampleMeans->GetDistribution();

				const double mathMean = sampleMeansDistribution->GetMean(), statMean = sampleMeans->GetMean(), errMean = statMean - mathMean;
				std::cout << "ǥ������� ������ ���: " << mathMean << "\nǥ������� ����� ���: " << statMean << "(���� " << errMean << ")\n";

				const double mathVariance = sampleMeansDistribution->GetVariance(), statVariance = sampleMeans->GetVariance(),
					errVariance = statVariance - mathVariance;
				std::cout << "ǥ������� ������ �л�: " << mathVariance << "\nǥ������� ����� �л�: " << statVariance << "(���� " << errVariance << ")\n";

				const double mathStandardDeviation = sampleMeansDistribution->GetStandardDeviation(), statStandardDeviation = sampleMeans->GetStandardDeviation(),
					errStandardDeviation = statStandardDeviation - mathStandardDeviation;
				std::cout << "ǥ������� ������ ǥ������: " << mathStandardDeviation << "\nǥ������� ����� ǥ������: " << statStandardDeviation << "(���� " << errStandardDeviation << ")\n";
				break;
			}

			case 9: {
				const int size = ReadInput<int>("ǥ���� ũ��");
				if (size < 1 || size > data->GetSize()) {
					std::cout << "�ùٸ��� ���� ũ���Դϴ�.\n";
					break;
				}

				StatSim::Population* const population = static_cast<StatSim::Population*>(data);
				const auto samples = population->GetSamples();
				if (samples.find(size) == samples.end()) {
					std::cout << "�ش� ũ���� ǥ���� �������� �ʽ��ϴ�.\n";
					break;
				}

				StatSim::Population* const sampleMeans = population->CreateSampleMeanPopulation(size);

				std::cin.ignore();
				const std::string path = ReadInput<std::string>("������ ���");
				sampleMeans->Save(path);
				break;
			}

			case 10: {
				const double k = ReadInput<double>("�ŷ� ���");

				StatSim::Population* const population = static_cast<StatSim::Population*>(data);
				const auto samples = population->GetSamples();
				if (samples.size() == 0) {
					std::cout << "ǥ���� �����ϴ�.\n";
					break;
				}

				const double mean = population->GetMean();

				int count = 0;
				int numberOfSamples = 0;
				double reliability = 0;
				for (const auto& [size, samples] : samples) {
					for (const auto& sample : samples) {
						const auto& [confidenceLevel, reliabilityTemp] = sample->GetConfidenceLevel(k);
						if (reliability == 0) {
							reliability = reliabilityTemp;
						}
						if (confidenceLevel.IsElement(mean)) {
							++count;
						}
						++numberOfSamples;
					}
				}

				const double hit = count / static_cast<double>(numberOfSamples), error = hit - reliability;
				std::cout << "�ŷڵ�: " << reliability << "\n���� Ȯ��: " << hit << "(���� " << error << ")\n";
				break;
			}
			}
		} else {
			switch (ReadAction("����", 5, "1. ���\n2. Ȯ������\n3. ǥ������\n4. ����� ����\n5. ������ ����")) {
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
				std::cout << "����� �����: " << statPopMean << "\n����� ǥ�����: " << statMean << "(���� " << errMean << ")\n";

				const double statPopVariance = population->GetVariance(), statVariance = data->GetVariance(), errVariance = statVariance - statPopVariance;
				std::cout << "����� ��л�: " << statPopVariance << "\n����� ǥ���л�: " << statVariance << "(���� " << errVariance << ")\n";

				const double statPopStandardDeviation = population->GetStandardDeviation(), statStandardDeviation = data->GetStandardDeviation(),
					errStandardDeviation = statStandardDeviation - statPopStandardDeviation;
				std::cout << "����� ��ǥ������: " << statPopStandardDeviation << "\n����� ǥ��ǥ������: " << statStandardDeviation << "(���� " << errStandardDeviation << ")\n";
				break;
			}

			case 4: {
				StatSim::Sample* const sample = static_cast<StatSim::Sample*>(data);
				StatSim::Population* const population = sample->GetPopulation();

				const double statPopMean = population->GetMean(), statMean = data->GetMean();
				const double statStandardDeviation = data->GetStandardDeviation();
				const double reliability = std::abs(StatSim::NormalCDF(statPopMean, statMean, statStandardDeviation) -
					StatSim::NormalCDF(2 * statMean - statPopMean, statMean, statStandardDeviation)) * 100;
				std::cout << "����� �����: " << statPopMean << "\n����� ǥ�����: " << statMean << "\n�ŷڵ� " << reliability << "% ���� ���� ����� ������� �����մϴ�.\n";
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
			std::cout << "�� �� ���� �����Դϴ�.";
		} else return result;
	} while (true);
}