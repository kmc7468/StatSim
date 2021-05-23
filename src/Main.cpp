#include <StatSim/Simulator.hpp>

#include <iostream>
#include <string_view>

template<typename T>
T ReadInput(const char* name);
int ReadAction(const char* name, int max, const std::string_view& actions);

int main() {
	StatSim::Simulator simulator;

	// 1. ������ ����
	const int populationSize = ReadInput<int>("�������� ũ��");

	switch (ReadAction("Ȯ������ ����", 2, "1. �̻�Ȯ������\n2. ����Ȯ������")) {
	case 1:
		switch (ReadAction("Ȯ������", 1, "1. ���׺���")) {
		case 1: {
			const int tryCount = ReadInput<int>("�õ� Ƚ��");
			const double probability = ReadInput<double>("����� Ȯ��");
			simulator.GeneratePopulation(populationSize, new StatSim::BinomialDistribution(tryCount, probability));
			break;
		}
		}
		break;

	case 2:
		switch (ReadAction("Ȯ������", 1, "1. ���Ժ���")) {
		case 1: {
			const double mean = ReadInput<double>("�����");
			const double standardDeviation = ReadInput<double>("��ǥ������");
			simulator.GeneratePopulation(populationSize, new StatSim::NormalDistribution(mean, standardDeviation));
			break;
		}
		}
		break;
	}

	// 2. ����
	while (true) {
		StatSim::Data* const data = simulator.GetSelectedData();
		StatSim::Distribution* const distribution = data->GetDistribution();

		std::cout << "----------\n���� ���: " << data->GetName() << '\n';
		if (simulator.IsPopulationSelected()) {
			switch (ReadAction("����", 6, "1. ���\n2. Ȯ������\n3. ��������\n4. ǥ�� ���\n5. ǥ�� ����\n6. ǥ�� ����")) {
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
				StatSim::Population* const population = static_cast<StatSim::Population*>(data);
				const auto samples = population->GetSamples();
				for (const auto& [size, samples] : samples) {
					std::cout << "n=" << size << "�� ǥ��\n";
					for (const auto* sample : samples) {
						std::cout << "- " << sample->GetName() << '\n';
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
				}
				break;
			}

			case 5: {
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
						firstSample = sample;
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

			case 6: {
				StatSim::Population* const population = static_cast<StatSim::Population*>(data);

				const int index = ReadInput<int>("ǥ���� ��ȣ");
				if (index < 0 || index > population->GetSampleCount()) {
					std::cout << "�ùٸ��� ���� ��ȣ�Դϴ�.\n";
					break;
				}

				simulator.SelectSample(index);
				break;
			}
			}
		} else {
			switch (ReadAction("����", 4, "1. ���\n2. Ȯ������\n3. ǥ������\n4. ������ ����")) {
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

			case 4:
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