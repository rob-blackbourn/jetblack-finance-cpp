#ifndef __jetblack__dates__calendars__target_hpp
#define __jetblack__dates__calendars__target_hpp

#include "dates/arithmetic.hpp"
#include "dates/business_days.hpp"
#include "dates/festivals.hpp"

#include <chrono>
#include <set>

namespace dates
{
	using namespace std::chrono;
	using namespace dates;
    
    namespace calendars
    {
        inline std::set<year_month_day> targetHolidays(year y)
        {
            std::set<year_month_day> holidays;

            auto easter_sunday = sys_days{easter(y)};

            // New Years Day
            holidays.insert(y/January/1d);
            // Good Friday
            holidays.insert(easter_sunday - days{2});
            // Easter Monday
            holidays.insert(easter_sunday + days{1});
            // Labour Day
            holidays.insert(y/May/1d);
            // Christmas Day
            holidays.insert(y/December/25d);
            // Good Will Day
            holidays.insert(y/December/26d);
            // December 31st, 1998, 1999, and 2001 only
            if (y == 1998y || y == 1999y || y == 2001y)
                holidays.insert(y/December/31d);

            return holidays;
        }

        inline std::set<year_month_day> targetHolidays(year startYear, year endYear)
        {
            std::set<year_month_day> holidays;

            for (auto y = startYear; y <= endYear; ++y)
            {
                auto holidaysInYear = targetHolidays(y);
                holidays.insert(holidaysInYear.begin(), holidaysInYear.end());

            }

            return holidays;
        }
    }
}
#endif // __jetblack__dates__calendars__target_hpp
