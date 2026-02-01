#include "rates/ir_swap_leg.hpp"

#include "dates/tenor.hpp"
#include "dates/schedules.hpp"

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	IrSwapLeg::IrSwapLeg(
		double notional,
		const year_month_day& firstAccrualDate,
		const year_month_day& maturityDate,
		EFrequency frequency,
		EStubType stubType,
		EDayCount dayCount,
		EDateRule dateRule,
		const std::set<year_month_day>& holidays)
		:	notional_(notional),
			firstAccrualDate_(firstAccrualDate),
			maturityDate_(maturityDate),
			frequency_(frequency),
			stubType_(stubType),
			dayCount_(dayCount),
			dateRule_(dateRule),
			schedule_(dates::generateSchedule(firstAccrualDate, maturityDate, frequency, stubType, dateRule, holidays))
	{
	}

	IrSwapLeg::IrSwapLeg(
		double notional,
		const year_month_day& firstAccrualDate,
		const time_unit_t& tenor,
		EFrequency frequency,
		EStubType stubType,
		EDayCount dayCount,
		EDateRule dateRule,
		const std::set<year_month_day>& holidays)
		:	IrSwapLeg(
				notional,
				firstAccrualDate,
				add(firstAccrualDate, tenor, isEndOfMonth(firstAccrualDate), dateRule, holidays),
				frequency,
				stubType,
				dayCount,
				dateRule,
				holidays)
	{
	}
}
