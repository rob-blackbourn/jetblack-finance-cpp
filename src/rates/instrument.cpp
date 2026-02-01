#include "rates/instrument.hpp"
#include "rates/yield_curve.hpp"


#include "maths/brent.hpp"

#include <limits>

namespace rates
{
	double Instrument::solveZeroRate(YieldCurve& curve, size_t index)
	{
		const double ERROR_TOLERANCE = std::numeric_limits<double>::epsilon();
		const unsigned int MAX_ITERATIONS = 100;

		return maths::brent::solve(
			[&](double rate)
			{
				curve.rate(index, rate);
				return value(curve);
			},
			-0.1, 1.0, MAX_ITERATIONS, ERROR_TOLERANCE
		);
	}
}
