#ifndef __jetblack__dates__cds_hpp
#define __jetblack__dates__cds_hpp

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

	inline
	year_month_day
	cdsRollDate(const year_month_day& PriceDate, const months& Months)
	{
		auto end_of_period = addMonths(PriceDate, Months, true);
		auto year = end_of_period.year();
		auto month_part = end_of_period.month();
		auto day_part = end_of_period.day();

		month m;
		if (month_part <= March)
    		m = March;
		else if (month_part <= June && month_part > March)
	    	m = June;
		else if (month_part <= September && month_part > June)
		    m = September;
		else if (month_part <= December && month_part > September)
		    m = December;
	        
		if (day_part > 20d)
		{
			switch (static_cast<unsigned int>(month_part))
			{
			case static_cast<unsigned int>(December):
				m = March;
				++year;
				break;
			case static_cast<unsigned int>(September):
				m += months{3};
				break;
			case static_cast<unsigned int>(June):
				m += months{3};
				break;
			case static_cast<unsigned int>(March):
				m += months{3};
				break;
			}
		}

		return year_month_day(year / m / 20d);
	}
}

#endif // __jetblack__dates__cds_hpp
