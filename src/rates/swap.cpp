#include "rates/swap.hpp"
#include "rates/yield_curve.hpp"

#include "maths/brent.hpp"

namespace rates
{
	double Swap::value(const YieldCurve& curve) const
	{
		double fixedPV = fixedLeg_.value(curve);
		double floatingPV = floatingLeg_.value(curve);
		return fixedPV - floatingPV;
	}

	double Swap::calculateZeroRate(const YieldCurve& curve) const
	{
		// Ignore the floating side - we only care about the fixed leg
		return fixedLeg_.calculateZeroRate(curve);
	}

	double Swap::solveZeroRate(YieldCurve& curve, size_t index) const
	{
		const double ERROR_TOLERANCE = 1e-11;
		const unsigned int MAX_ITERATIONS = 50;

		double lastRate = (curve.points().empty() ? 0.33 : curve.points().back().rate());

		Swap swap { *this };
		return maths::brent::solve(
			[&curve, &swap, index](double rate)
			{
				curve.rate(index, rate);
				swap.floatingLeg().fixFloatingRates(curve, swap_fixing_t());
				return swap.value(curve);
			},
			-0.1, std::max(0.10, 3.0 * lastRate), MAX_ITERATIONS, ERROR_TOLERANCE
		);
	}

	bool compareSwaps(const Swap& s1, const Swap& s2)
	{
		return s1.endDate() < s2.endDate();
	}

	double Swap::solveSwapRate(YieldCurve& curve, const swap_fixing_t& swap_fixing)
	{
		const double ERROR_TOLERANCE = 1e-11;
		const unsigned int MAX_ITERATIONS = 100;

		swap_fixing_t fixing { swap_fixing };

		return maths::brent::solve(
			[&curve, &fixing, this](double rate)
			{
				fixedLeg().rate(rate);
				fixing = floatingLeg().fixFloatingRates(curve, fixing);
				return value(curve);

			},
			-0.1, 1.0, MAX_ITERATIONS, ERROR_TOLERANCE
		);
	}
}