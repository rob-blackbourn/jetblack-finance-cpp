#include "rates/ir_swap.hpp"
#include "rates/yield_curve.hpp"

#include "maths/brent.hpp"

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
		const year_month_day& startDate,
		const time_unit_t& tenor,
		EFrequency frequency,
		EStubType stubType,
		EDateRule dateRule,
		const std::set<year_month_day>& holidays,
		EDayCount dayCount,
		double notional,
		double fixedRate,
		const time_unit_t& fixLag,
		double floatingSpread)
		:	fixedLeg_(
				IrSwapLegFixed(
					startDate,
					tenor,
					frequency,
					stubType,
					dateRule,
					holidays,
					dayCount,
					notional,
					fixedRate)),
			floatingLeg_(
				IrSwapLegFloating(
					startDate,
					tenor,
					frequency,
					stubType,
					dateRule,
					holidays,
					dayCount,
					notional,
					fixLag,
					floatingSpread))
	{		
	}

	IrSwap::IrSwap(
		const year_month_day& startDate,
		const year_month_day& endDate,
		EFrequency frequency,
		EStubType stubType,
		EDateRule dateRule,
		const std::set<year_month_day>& holidays,
		EDayCount dayCount,
		double notional,
		double fixedRate,
		const time_unit_t& fixLag,
		double floatingSpread)
		:	fixedLeg_(
				IrSwapLegFixed(
					startDate,
					endDate,
					frequency,
					stubType,
					dateRule,
					holidays,
					dayCount,
					notional,
					fixedRate)),
			floatingLeg_(
				IrSwapLegFloating(
					startDate,
					endDate,
					frequency,
					stubType,
					dateRule,
					holidays,
					dayCount,
					notional,
					fixLag,
					floatingSpread))
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

	double IrSwap::solveZeroRate(YieldCurve& curve, size_t index)
	{
		const double ERROR_TOLERANCE = 1e-11;
		const unsigned int MAX_ITERATIONS = 100;

		IrSwap swap { *this };
		double lastRate = curve.points().empty() ? 0.33 : curve.points().back().rate();

		return maths::brent::solve(
			[&](double rate)
			{
				curve.rate(index, rate);
				floatingLeg().reset(curve, std::optional<double>(), std::optional<double>());
				return value(curve);
			},
			-0.1, std::max(0.10, 3.0 * lastRate), MAX_ITERATIONS, ERROR_TOLERANCE
		);
	}

	bool compareIrSwaps(const IrSwap& s1, const IrSwap& s2)
	{
		return s1.endDate() < s2.endDate();
	}

	double IrSwap::solveSwapRate(
		const YieldCurve& curve,
		const std::optional<double>& first_fixing,
		const std::optional<double>& second_fixing)
	{
		const double ERROR_TOLERANCE = 1e-11;
		const unsigned int MAX_ITERATIONS = 100;

		floatingLeg_.reset(curve, first_fixing, second_fixing);

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