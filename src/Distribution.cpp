#include <StatSim/Distribution.hpp>

#include <cmath>

namespace StatSim {
	double Distribution::GetVariance() const {
		return std::pow(GetStandardDeviation(), 2);
	}
	double Distribution::GetStandardDeviation() const {
		return std::sqrt(GetVariance());
	}
}