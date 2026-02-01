#include "rates/ir_swap.hpp"
#include "rates/yield_curve.hpp"

#include "maths/brent.hpp"

#include <limits>

namespace rates
{
	using namespace std::chrono;

	IrSwap::IrSwap(
		const IrSwapLegFixed& fixedLeg,
		const IrSwapLegFloating& floatingLeg)
		:	fixedLeg_(fixedLeg),
			floatingLeg_(floatingLeg)
	{
	}

	IrSwap::IrSwap(
		double notional,
		double fixedRate,
		double floatingSpread,
		const year_month_day& firstAccrualDate,
		const time_unit_t& tenor,
		EFrequency frequency,
		EStubType stubType,
		EDateRule dateRule,
		EDayCount dayCount,
		const time_unit_t& fixLag,
		const std::set<year_month_day>& holidays)
		:	fixedLeg_(
				IrSwapLegFixed(
					notional,
					fixedRate,
					firstAccrualDate,
					tenor,
					frequency,
					stubType,
					dateRule,
					dayCount,
					holidays)),
			floatingLeg_(
				IrSwapLegFloating(
					notional,
					floatingSpread,
					firstAccrualDate,
					tenor,
					frequency,
					stubType,
					dateRule,
					dayCount,
					fixLag,
					holidays))
	{		
	}

	IrSwap::IrSwap(
		double notional,
		double fixedRate,
		double floatingSpread,
		const year_month_day& firstAccrualDate,
		const year_month_day& maturityDate,
		EFrequency frequency,
		EStubType stubType,
		EDateRule dateRule,
		EDayCount dayCount,
		const time_unit_t& fixLag,
		const std::set<year_month_day>& holidays)
		:	fixedLeg_(
				IrSwapLegFixed(
					notional,
					fixedRate,
					firstAccrualDate,
					maturityDate,
					frequency,
					stubType,
					dateRule,
					dayCount,
					holidays)),
			floatingLeg_(
				IrSwapLegFloating(
					notional,
					floatingSpread,
					firstAccrualDate,
					maturityDate,
					frequency,
					stubType,
					dateRule,
					dayCount,
					fixLag,
					holidays))
	{		
	}

	double IrSwap::value(const year_month_day& valueDate, const YieldCurve& curve) const
	{
		double fixedPV = fixedLeg_.value(valueDate, curve);
		double floatingPV = floatingLeg_.value(valueDate, curve);
		return fixedPV - floatingPV;
	}

	double IrSwap::value(const YieldCurve& curve) const
	{
		return value(curve.valueDate(), curve);
	}

	double IrSwap::calculateZeroRate(const YieldCurve& curve) const
	{
		// Ignore the floating side - we only care about the fixed leg
		return fixedLeg_.calculateZeroRate(curve);
	}

	double IrSwap::solveSwapRate(
		const YieldCurve& curve,
		const std::optional<double>& first_fixing,
		const std::optional<double>& second_fixing)
	{
		const double ERROR_TOLERANCE = std::numeric_limits<double>::epsilon();
		const unsigned int MAX_ITERATIONS = 100;

		return maths::brent::solve(
			[&](double rate)
			{
				this->fixedLeg().rate(rate);
				return this->value(curve);
			},
			-0.1, 1.0, MAX_ITERATIONS, ERROR_TOLERANCE
		);
	}
}