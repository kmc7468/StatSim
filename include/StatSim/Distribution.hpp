#pragma once

#include <string>

namespace StatSim {
	class Distribution {
	public:
		Distribution() = delete;
		Distribution(const Distribution&) = delete;
		virtual ~Distribution() = default;

	public:
		Distribution& operator=(const Distribution&) = delete;

	public:
		virtual std::string GetName() const = 0;
		virtual std::string GetExpression() const = 0;

		virtual double GetMean() const = 0;
		virtual double GetVariance() const;
		virtual double GetStandardDeviation() const;
	};
}