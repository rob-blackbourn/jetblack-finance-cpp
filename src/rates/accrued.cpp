#include "rates/accrued.hpp"

#include <ranges>

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	double accrued(
		const year_month_day& valueDate,
		const std::vector<year_month_day>& schedule,
		EDayCount dayCount,
		double rate,
		double notional)
	{
		if (valueDate <= schedule.front() || valueDate >= schedule.back())
			return 0.0;

		for (const auto &[startDate, endDate] : std::views::zip(schedule, schedule | std::views::drop(1)))
		{
			if (valueDate >= startDate && valueDate < endDate)
			{
				return notional * rate * yearFrac(startDate, valueDate, dayCount);
			}
		}

		return 0.0;
	}

	double accrued(
		const year_month_day& valueDate,
		const std::vector<year_month_day>& schedule,
		EDayCount dayCount,
		const std::vector<double>& fixings,
		double notional)
	{
		if (valueDate <= schedule.front() || valueDate >= schedule.back())
			return 0.0;

		for (const auto &[startDate, endDate, fixing] : std::views::zip(schedule, schedule | std::views::drop(1), fixings))
		{
			if (valueDate >= startDate && valueDate < endDate)
			{
				return notional * fixing * yearFrac(startDate, valueDate, dayCount);
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
			return 0.0;

		for (const auto &[startDate, endDate, fixing] : std::views::zip(schedule, schedule | std::views::drop(1), fixings))
		{
			if (valueDate >= startDate && valueDate < endDate)
			{
				return notional * fixing.rate() * yearFrac(startDate, valueDate, dayCount);
			}
		}

		return 0.0;
	}
}