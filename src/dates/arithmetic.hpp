#ifndef __jetblack__dates__arithmetic_hpp
#define __jetblack__dates__arithmetic_hpp

#include <cstring>

#include <algorithm>
#include <chrono>
#include <set>
#include <string>
#include <type_traits>
#include <vector>

namespace dates
{
    using namespace std::chrono;

	inline weekday dayOfWeek(const year_month_day& date)
	{
		return weekday { sys_days(date) };
	}

    inline days dayOfYear(const year_month_day& date)
	{
		return sys_days(date) - sys_days{date.year()/1/0};
	}

    inline day lastDayOfMonth(const year& y, const month& m)
	{
		return year_month_day{y/m/last}.day();
	}

	inline days daysInMonth(const year& y, const month& m)
	{
        return days {static_cast<unsigned int>(lastDayOfMonth(y, m))};
	}

	inline days daysInYear(const year& y)
	{
		return days { y.is_leap() ? 366 : 365 };
	}

	inline bool isEndOfMonth(const year_month_day& date)
	{
		return date.day() == lastDayOfMonth(date.year(), date.month());
	}

	inline year_month_day moveToEndOfMonth(const year_month_day& date)
	{
		return sys_days {year_month_day {date.year() / date.month() / last}};
	}

	inline year_month_day addMonths(const year_month_day& date, const months& m, bool eom)
	{
		auto date1 = date + m;
		if (!date1.ok() || (eom && isEndOfMonth(date)))
		{
			date1 = moveToEndOfMonth(date1);
		}

		return date1;
	}

	inline year_month_day addYears(const year_month_day& date, const years& y, bool eom)
	{
		return addMonths(date, duration_cast<months>(y), eom);
	}

	inline year_month_day addWeeks(const year_month_day& date, const weeks& w)
	{
		return sys_days{date} + w;
	}

	inline year_month_day addNthDayOfWeek(const year_month_day& date, int nth, const weekday& dow, bool strictly_different)
	{
		if (nth == 0) {
			return date;
		}

		auto diff = dow - dayOfWeek(date);

		if (diff == days{0} && strictly_different)
			nth += nth >= 0 ? 1 : -1;

		// forwards
		if (nth > 0) {
			// If diff = 0 below, the input date is the 1st DOW already, no adjustment
			// is required. The 'diff' is the adjustment from the input date
			// required to get to the first DOW matching the 'dow_index' given.

			if (diff < days{0})
				diff += days{7};

			auto adjusted_start_date = sys_days{date} + diff;
			auto end_date = adjusted_start_date + weeks{nth - 1};
			return end_date;
		}
		// backwards
		else {
			// If diff = 0 below, the input date is the 1st DOW already, no adjustment
			// is required. The 'diff' is the adjustment from the input date
			// required to get to the first DOW matching the 'dow_index' given.

			if (diff > days{0})
				diff -= days{7};

			auto adjusted_start_date = sys_days{date} + diff;
			auto end_date = adjusted_start_date + weeks{nth + 1};
			return end_date;
		}
	}
}

#endif // __jetblack__dates__arithmetic_hpp
