#include <StatSim/Math.hpp>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>

#include <cmath>
#include <numbers>

namespace StatSim {
	double BinomialPMF(int tryCount, int occurCount, double probability) {
		const auto numberOfCases = [](int n, int r) {
			if (n - r < r) {
				r = n - r;
			}

			boost::multiprecision::cpp_int result = 1;
			for (int i = 0; i < r; ++i) {
				result *= n - i;
			}
			for (int i = 0; i < r; ++i) {
				result /= i + 1;
			}
			return result;
		}(tryCount, occurCount);

		const boost::multiprecision::cpp_bin_float_100 probabilityEx(probability);
		const auto occurProbability = boost::multiprecision::pow(probabilityEx, occurCount);
		const auto notOccurProbability = boost::multiprecision::pow(1 - probabilityEx, tryCount - occurCount);

		const auto result = static_cast<boost::multiprecision::cpp_bin_float_100>(numberOfCases) * occurProbability * notOccurProbability;
		return static_cast<double>(result);
	}
	double NormalCDF(double value, double mean, double standardDeviation) {
		return 0.5 * std::erfc((mean - value) / standardDeviation / std::numbers::sqrt2_v<double>);
	}
}