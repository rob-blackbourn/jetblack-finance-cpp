#ifndef __jetblack__rates__accrued_hpp
#define __jetblack__rates__accrued_hpp

#include <chrono>
#include <vector>

#include "dates/terms.hpp"

#include "rates/dated_rate.hpp"

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	class YieldCurve;

	double accrued(
		const year_month_day& valueDate,
		const std::vector<year_month_day>& schedule,
		EDayCount dayCount,
		double rate,
		double notional);

	double accrued(
		const year_month_day& valueDate,
		const std::vector<year_month_day>& schedule,
		EDayCount dayCount,
		const std::vector<double>& fixingRates,
		double notional);

	double accrued(
		const year_month_day& valueDate,
		const std::vector<year_month_day>& schedule,
		EDayCount dayCount,
		const std::vector<DatedRate>& fixings,
		double notional);
}

#endif // __jetblack__rates__accrued_hpp
