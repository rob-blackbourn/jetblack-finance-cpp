#include "rates/ir_swap_leg.hpp"

#include "dates/tenor.hpp"
#include "dates/schedules.hpp"

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	IrSwapLeg::IrSwapLeg(
		const year_month_day& startDate,
		const year_month_day& endDate,
		double notional,
		EFrequency frequency,
		EStubType stubType,
		EDayCount dayCount,
		EDateRule dateRule,
		const std::set<year_month_day>& holidays)
		:	startDate_(startDate),
			endDate_(endDate),
			notional_(notional),
			frequency_(frequency),
			stubType_(stubType),
			dayCount_(dayCount),
			dateRule_(dateRule),
			schedule_(dates::generateSchedule(startDate, endDate, frequency, stubType, dateRule, holidays))
	{
	}

	IrSwapLeg::IrSwapLeg(
		const year_month_day& startDate,
		const time_unit_t& tenor,
		double notional,
		EFrequency frequency,
		EStubType stubType,
		EDayCount dayCount,
		EDateRule dateRule,
		const std::set<year_month_day>& holidays)
		:	IrSwapLeg(
				startDate,
				add(startDate, tenor, isEndOfMonth(startDate), dateRule, holidays),
				notional,
				frequency,
				stubType,
				dayCount,
				dateRule,
				holidays)
	{
	}
}
