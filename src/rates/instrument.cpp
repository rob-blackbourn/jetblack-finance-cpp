#include "rates/instrument.hpp"
#include "rates/yield_curve.hpp"

#include "maths/brent.hpp"

namespace rates
{
	double Instrument::solveZeroRate(
		YieldCurve& curve,
		unsigned int maxIterations,
		double errorTolerance) const
	{
		return maths::brent::solve(
			[&](double rate)
			{
				curve.setLastRate(rate);
				return value(curve);
			},
			-0.1, 1.0, maxIterations, errorTolerance
		);
	}
}
