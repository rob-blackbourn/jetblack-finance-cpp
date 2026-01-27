#include "rates/accrued.hpp"

#include <ranges>

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	static double accrued(
		const year_month_day& startDate,
		const year_month_day& valueDate,
		EDayCount dayCount,
		double rate,
		double notional)
	{
		// The accrued is the amount of the notional between the start of the
		// period and the cashflow date multiplied by the rate.
		auto t = yearFrac(startDate, valueDate, dayCount);
		return notional * rate * t;		
	}

	double accrued(
		const year_month_day& valueDate,
		const std::vector<year_month_day>& schedule,
		EDayCount dayCount,
		double rate,
		double notional)
	{
		if (valueDate <= schedule.front() || valueDate >= schedule.back())
		{
			// The value date is outside the schedule of payments, so there
			// is no accrued.
			return 0.0;
		}

		for (const auto &[startDate, endDate] : std::views::zip(schedule, schedule | std::views::drop(1)))
		{
			if (valueDate >= startDate && valueDate < endDate)
			{
				// The value date is within this cashflow period.
				return accrued(startDate, valueDate, dayCount, rate, notional);
			}
		}

		return 0.0;
	}

	double accrued(
		const year_month_day& valueDate,
		const std::vector<year_month_day>& schedule,
		EDayCount dayCount,
		const std::vector<double>& fixingRates,
		double notional)
	{
		if (valueDate <= schedule.front() || valueDate >= schedule.back())
		{
			// The value date is outside the schedule of payments, so there
			// is no accrued.
			return 0.0;
		}

		for (
			const auto &[startDate, endDate, rate]
			: std::views::zip(
				schedule,
				schedule | std::views::drop(1),
				fixingRates))
		{
			if (valueDate >= startDate && valueDate < endDate)
			{
				// The value date is within this cashflow period.
				return accrued(startDate, valueDate, dayCount, rate, notional);
			}
		}

		return 0.0;
	}

	double accrued(
		const year_month_day& valueDate,
		const std::vector<year_month_day>& schedule,
		EDayCount dayCount,
		const std::vector<DatedRate>& fixings,
		double notional)
	{
		if (valueDate <= schedule.front() || valueDate >= schedule.back())
		{
			// The value date is outside the schedule of payments, so there
			// is no accrued.
			return 0.0;
		}

		for (const auto &[startDate, endDate, fixing] : std::views::zip(schedule, schedule | std::views::drop(1), fixings))
		{
			if (valueDate >= startDate && valueDate < endDate)
			{
				// The value date is within this cashflow period.
				return accrued(startDate, valueDate, dayCount, fixing.rate(), notional);
			}
		}

		return 0.0;
	}
}