#include "rates/accrued.hpp"
#include "rates/ir_swap_leg_floating.hpp"
#include "rates/ir_swap.hpp"
#include "rates/value.hpp"
#include "rates/yield_curve.hpp"

#include <ranges>

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	static std::vector<DatedRate> createFixings(
		const std::vector<year_month_day>& schedule,
		double rate,
		const time_unit_t& fixLag,
		const std::set<year_month_day>& holidays)
	{
		return schedule
			| std::views::drop(1) // No fixing is required for the start date.
			| std::views::transform(
				[rate, &fixLag, &holidays](const year_month_day& d)
				{
					return DatedRate { add(d, -fixLag, true, EDateRule::Preceding, holidays), rate };
				})
			| std::ranges::to<std::vector<DatedRate>>();
	}

	IrSwapLegFloating::IrSwapLegFloating(
		const year_month_day& startDate,
		const time_unit_t& tenor,
		EFrequency frequency,
		EStubType stubType,
		EDateRule dateRule,
		const std::set<year_month_day>& holidays,
		EDayCount dayCount,
		double notional,
		const time_unit_t& fixLag,
		double spread)
		:	IrSwapLeg(startDate, tenor, notional, frequency, stubType, dayCount, dateRule, holidays),
			spread_(spread),
			fixLag_(fixLag),
			fixings_(createFixings(schedule_, 0.0, fixLag_, holidays))
	{
	}

	IrSwapLegFloating::IrSwapLegFloating(
		const year_month_day& startDate,
		const year_month_day& endDate,
		EFrequency frequency,
		EStubType stubType,
		EDateRule dateRule,
		const std::set<year_month_day>& holidays,
		EDayCount dayCount,
		double notional,
		const time_unit_t& fixLag,
		double spread)
		:	IrSwapLeg(startDate, endDate, notional, frequency, stubType, dayCount, dateRule, holidays),
			spread_(spread),
			fixLag_(fixLag)
	{
	}

	double IrSwapLegFloating::accrued(const year_month_day& valueDate) const
	{
		return rates::accrued(valueDate, schedule_, dayCount_, fixings_, notional_);
	}

	double IrSwapLegFloating::value(const year_month_day& valueDate, const YieldCurve& curve) const
	{
		return rates::value(valueDate, curve, schedule_, dayCount_, fixings_, notional_);
	}

	double IrSwapLegFloating::value(const YieldCurve& curve) const
	{
		return value(curve.valueDate(), curve);
	}

	std::pair<std::optional<double>,std::optional<double>> IrSwapLegFloating::getCurrentFixings(const year_month_day& valueDate) const
	{
		for (const auto&[startDate, endDate, prevFixing, nextFixing] : std::views::zip(schedule_, schedule_ | std::views::drop(1), fixings_, fixings_ | std::views::drop(1)))
		{
			if (valueDate >= startDate && valueDate < endDate)
			{
				return {prevFixing.rate(), nextFixing.rate()};
			}
		}

		return {};
	}

	// Create the fixings from the supplied fixes and from the yield curve.
	void IrSwapLegFloating::reset(
		const YieldCurve& curve,
		const std::optional<double>& prevFixing,
		const std::optional<double>& nextFixing)
	{
		for (auto&& [startDate, endDate, fixing] : std::views::zip(schedule_, schedule_ | std::views::drop(1), fixings_))
		{
			double fix = 0.0;
			if (curve.valueDate() >= startDate)
			{
				// Cashflow started in the past.
				fix = (!prevFixing
					? curve.fix(curve.valueDate(), fixing.date(), dayCount_)
					: *prevFixing);
			}
			else if (curve.valueDate() >= fixing.date())
			{
				// Cashflow started in the future, but the fix date is in the past.
				fix = (!nextFixing
					? curve.fix(curve.valueDate(), fixing.date(), dayCount_)
					: *nextFixing);
			}
			else
			{
				// fix occurs in the future.
				fix = curve.fix(startDate, fixing.date(), dayCount_);
			}

			fixing.rate(fix + spread_);
		}
	}
}