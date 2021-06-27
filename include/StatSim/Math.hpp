#pragma once

namespace StatSim {
	double BinomialPMF(int tryCount, int occurCount, double probability);
	double NormalCDF(double value, double mean, double standardDeviation);
}