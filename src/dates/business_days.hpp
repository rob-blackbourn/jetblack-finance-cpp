#ifndef __jetblack__dates__business_days_hpp
#define __jetblack__dates__business_days_hpp

#include <cstring>

#include <algorithm>
#include <chrono>
#include <set>
#include <string>
#include <type_traits>
#include <vector>

#include "dates/arithmetic.hpp"

namespace dates
{
    using namespace std::chrono;

	inline bool isWeekend(const year_month_day& date)
	{
		auto dow = dayOfWeek(date);
		return dow == Saturday || dow == Sunday;
	}

	inline
	bool isHoliday(const year_month_day& date, const std::set<year_month_day>& holidays)
	{
		return holidays.contains(date);
	}

	inline
	bool isBusinessDay(const year_month_day& date, const std::set<year_month_day>& holidays)
	{
		return (!(isWeekend(date) || isHoliday(date, holidays)));
	}

	inline
	year_month_day addBusinessDays(const year_month_day& date, days businessDays, const std::set<year_month_day>& holidays = std::set<year_month_day>{})
	{
		auto businessDate = sys_days{date};
		auto sign = days{businessDays.count() > 0 ? 1 : -1};

        auto remainingDays = businessDays;

		while (remainingDays.count() != 0)
		{
			// Move a day in the appropriate direction.
			businessDate += sign;

			// Ensure this day is a business day.
			while (!isBusinessDay(businessDate, holidays))
				businessDate += sign;

			// This day is done.
			remainingDays -= sign;
		}

		return businessDate;
	}

	inline std::chrono::year_month_day nearestBusinessDay(
		const std::chrono::year_month_day& date,
		bool prefer_forward,
		const std::set<std::chrono::year_month_day> holidays)
	{
		// Find the nearest business day to a given date.
		// 
		// Args:
		// 	date (datetime.date): The date.
		// 	prefer_forward (bool, optional): If True prefer the nearest business day
		// 		in the future. Defaults to True.
		// 	cal (AbstractCalendar, optional): The holiday calendar. Defaults to
		// 		WEEKEND_CALENDAR.
		// 
		// Returns:
		// 	datetime.date: The nearest business day to a given date.
		using namespace std::chrono;

		if (isBusinessDay(date, holidays))
			return date;

		auto one_day = days(1);
		auto forward_date = sys_days{date} + one_day;
		auto backward_date = sys_days{date} - one_day;

		while (true)
		{
			auto is_forward_ok = isBusinessDay(forward_date, holidays);
			auto is_backward_ok = isBusinessDay(backward_date, holidays);
			if (is_forward_ok and (prefer_forward or not is_backward_ok))
				return forward_date;
			else if (is_backward_ok)
				return backward_date;
			forward_date += one_day;
			backward_date -= one_day;
		}
	}

	inline year_month_day immDate(year y, month m)
	{
    	return addNthDayOfWeek(y/m/1d, 3, Wednesday, true);
	}

	inline year_month_day immDate(year_month ym)
	{
    	return addNthDayOfWeek(ym/1d, 3, Wednesday, true);
	}
}

#endif // __jetblack__dates__business_days_hpp
