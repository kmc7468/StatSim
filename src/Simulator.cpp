#include <StatSim/Simulator.hpp>

#include <iostream>

namespace StatSim {
	Simulator::~Simulator() {
		delete m_Population;
	}

	void Simulator::GeneratePopulation(int size, Distribution* distribution) {
		m_Population = new Population(size, distribution);
		SelectPopulation();
	}

	Data* Simulator::GetSelectedData() noexcept {
		return m_SelectedData;
	}
	void Simulator::SelectPopulation() noexcept {
		m_SelectedData = m_Population;
	}
	void Simulator::SelectSample(int index) {
		m_SelectedData = m_Population->GetSample(index);
	}
	bool Simulator::IsPopulationSelected() const noexcept {
		return m_SelectedData == m_Population;
	}

	void Simulator::PrintSelectedData() {
		for (int i = 0; i < m_SelectedData->GetSize(); ++i) {
			std::cout << (*m_SelectedData)[i] << ' ';
		}
	}
}