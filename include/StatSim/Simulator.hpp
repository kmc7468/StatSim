#pragma once

#include <StatSim/Data.hpp>
#include <StatSim/Distribution.hpp>

#include <string>

namespace StatSim {
	class Simulator final {
	private:
		Population* m_Population = nullptr;
		Data* m_SelectedData = nullptr;

	public:
		Simulator() noexcept = default;
		Simulator(const Simulator&) = delete;
		~Simulator();

	public:
		Simulator& operator=(const Simulator&) = delete;

	public:
		void GeneratePopulation(int size, Distribution* distribution);

		Data* GetSelectedData() noexcept;
		void SelectPopulation() noexcept;
		void SelectSample(int index);
		bool IsPopulationSelected() const noexcept;

		void PrintSelectedData();
	};
}