#include "rates/accrued.hpp"

#include <optional>
#include <ranges>
#include <stdexcept>

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	static double accrued(
		const year_month_day& firstAccrualDate,
		const year_month_day& valueDate,
		EDayCount dayCount,
		double rate,
		double notional)
	{
		// The accrued is the amount of the notional between the start of the
		// period and the cashflow date multiplied by the rate.
		auto t = yearFrac(firstAccrualDate, valueDate, dayCount);
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

		for (const auto &[firstAccrualDate, endDate] : std::views::zip(schedule, schedule | std::views::drop(1)))
		{
			if (valueDate >= firstAccrualDate && valueDate < endDate)
			{
				// The value date is within this cashflow period.
				return accrued(firstAccrualDate, valueDate, dayCount, rate, notional);
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
			const auto &[firstAccrualDate, endDate, rate]
			: std::views::zip(
				schedule,
				schedule | std::views::drop(1),
				fixingRates))
		{
			if (valueDate >= firstAccrualDate && valueDate < endDate)
			{
				// The value date is within this cashflow period.
				return accrued(firstAccrualDate, valueDate, dayCount, rate, notional);
			}
		}

		return 0.0;
	}
}