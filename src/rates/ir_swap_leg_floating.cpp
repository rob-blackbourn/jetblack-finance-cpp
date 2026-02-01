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

	static std::vector<year_month_day> createFixings(
		const std::vector<year_month_day>& schedule,
		const time_unit_t& fixLag,
		const std::set<year_month_day>& holidays)
	{
		return schedule
			| std::views::drop(1) // No fixing is required for the start date.
			| std::views::transform(
				[&fixLag, &holidays](const year_month_day& d)
				{
					return add(d, -fixLag, true, EDateRule::Preceding, holidays);
				})
			| std::ranges::to<std::vector<year_month_day>>();
	}

	IrSwapLegFloating::IrSwapLegFloating(
		double notional,
		double spread,
		const year_month_day& startDate,
		const time_unit_t& tenor,
		EFrequency frequency,
		EStubType stubType,
		EDateRule dateRule,
		EDayCount dayCount,
		const time_unit_t& fixLag,
		const std::set<year_month_day>& holidays)
		:	IrSwapLeg(
				notional,
				startDate,
				tenor,
				frequency,
				stubType,
				dayCount,
				dateRule,
				holidays),
			spread_(spread),
			fixLag_(fixLag),
			fixingSchedule_(createFixings(schedule_, fixLag_, holidays))
	{
	}

	IrSwapLegFloating::IrSwapLegFloating(
		double notional,
		double spread,
		const year_month_day& startDate,
		const year_month_day& endDate,
		EFrequency frequency,
		EStubType stubType,
		EDateRule dateRule,
		EDayCount dayCount,
		const time_unit_t& fixLag,
		const std::set<year_month_day>& holidays)
		:	IrSwapLeg(
				notional,
				startDate,
				endDate,
				frequency,
				stubType,
				dayCount,
				dateRule,
				holidays),
			spread_(spread),
			fixLag_(fixLag)
	{
	}

	std::vector<double> IrSwapLegFloating::getFixingRates(const YieldCurve& curve) const
	{
		return std::ranges::zip_view(schedule_, fixingSchedule_)
			| std::views::transform(
				[&](auto&& x)
				{
					auto& [startDate, fixingDate] = x;
					return curve.fix(startDate, fixingDate, dayCount_);
				})
			| std::ranges::to<std::vector<double>>();
	}

	double IrSwapLegFloating::accrued(const YieldCurve& curve, const year_month_day& valueDate) const
	{
		auto fixingRates = getFixingRates(curve);
		return rates::accrued(valueDate, schedule_, dayCount_, fixingRates, notional_);
	}

	double IrSwapLegFloating::value(const year_month_day& valueDate, const YieldCurve& curve) const
	{
		auto fixingRates = getFixingRates(curve);
		return rates::value(valueDate, curve, schedule_, dayCount_, fixingRates, notional_);
	}

	double IrSwapLegFloating::value(const YieldCurve& curve) const
	{
		return value(curve.valueDate(), curve);
	}

	std::pair<std::optional<double>,std::optional<double>>
	IrSwapLegFloating::getCurrentFixings(const YieldCurve& curve, const year_month_day& valueDate) const
	{
		auto fixingRates = getFixingRates(curve);
		for (const auto&[startDate, endDate, prevFixing, nextFixing]
			: std::views::zip(
				schedule_,
				schedule_ | std::views::drop(1),
				fixingRates,
				fixingRates | std::views::drop(1)))
		{
			if (valueDate >= startDate && valueDate < endDate)
			{
				return {prevFixing, nextFixing};
			}
		}

		return {};
	}
}