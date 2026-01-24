#include "rates/swap_leg.hpp"
#include "rates/cashflow.hpp"
#include "rates/accrued.hpp"

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	SwapLeg::SwapLeg(
		double notional,
		const year_month_day& startDate,
		const time_unit_t& tenor,
		EFrequency frequency,
		EStubType stubType,
		EDayCount dayCount,
		EDateRule dateRule,
		const std::set<year_month_day>& holidays)
		:	SwapLeg(
				notional,
				startDate,
				add(startDate, tenor, isEndOfMonth(startDate), dateRule, holidays),
				frequency,
				stubType,
				dayCount,
				dateRule,
				holidays)
	{
	}

	SwapLeg::SwapLeg(
		double notional,
		const year_month_day& startDate,
		const year_month_day& endDate,
		EFrequency frequency,
		EStubType stubType,
		EDayCount dayCount,
		EDateRule dateRule,
		const std::set<year_month_day>& holidays)
		:	notional_(notional),
			startDate_(startDate),
			endDate_(endDate),
			frequency_(frequency),
			stubType_(stubType),
			dayCount_(dayCount),
			dateRule_(dateRule),
			holidays_(holidays)
	{
	}

	double SwapLeg::accrued(const std::chrono::year_month_day& valueDate) const
	{
		return rates::calculateAccrued(valueDate, interestCashflows_);
	}

	double SwapLeg::value(const YieldCurve& curve) const
	{
		return rates::valueCashflows(interestCashflows_, curve) + rates::valueCashflows(principalCashflows_, curve);
	}
}