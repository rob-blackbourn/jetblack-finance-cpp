#ifndef __jetblack__rates__value_hpp
#define __jetblack__rates__value_hpp

#include <chrono>
#include <vector>

#include "dates/terms.hpp"
#include "dates/frequency.hpp"

#include "rates/dated_rate.hpp"

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	class YieldCurve;

	double value(
		const year_month_day& valueDate,
		const YieldCurve& curve,
		const std::vector<year_month_day>& schedule,
		EDayCount dayCount,
		double couponRate,
		double notional);

	double value(
		const year_month_day& valueDate,
		const YieldCurve& curve,
		const std::vector<year_month_day>& schedule,
		EDayCount dayCount,
		const std::vector<double>& fixings,
		double notional);

	double value(
		const year_month_day& valueDate,
		const YieldCurve& curve,
		const std::vector<year_month_day>& schedule,
		EDayCount dayCount,
		const std::vector<DatedRate>& fixings,
		double notional);

	double value(
		const year_month_day& valueDate,
		double yield,
		const std::vector<year_month_day>& schedule,
		EDayCount dayCount,
		double couponRate,
		double notional,
		EFrequency frequency);
}

#endif // __jetblack__rates__value_hpp
