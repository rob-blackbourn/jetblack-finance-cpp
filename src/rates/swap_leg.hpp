#ifndef __jetblack__rates__swap_leg_hpp
#define __jetblack__rates__swap_leg_hpp

#include <chrono>
#include <set>
#include <vector>

#include "dates/schedules.hpp"
#include "dates/terms.hpp"
#include "dates/frequency.hpp"

#include "rates/cashflow.hpp"

namespace rates
{
	using namespace std::chrono;
	using namespace rates;

	class YieldCurve;

	class SwapLeg
	{
	public:
		SwapLeg() = default;
		
		SwapLeg(
			double notional,
			const year_month_day& startDate,
			const time_unit_t& tenor,
			EFrequency frequency,
			EStubType stubType,
			EDayCount dayCount,
			EDateRule dateRule,
			const std::set<year_month_day>& holidays);

		SwapLeg(
			double notional,
			const year_month_day& startDate,
			const year_month_day& endDate,
			EFrequency frequency,
			EStubType stubType,
			EDayCount dayCount,
			EDateRule dateRule,
			const std::set<year_month_day>& holidays);

		double value(const YieldCurve& curve) const;
		double accrued(const year_month_day& valueDate) const;

		EFrequency frequency() const { return frequency_; }
		EDateRule dateRule() const { return dateRule_; }
		const std::set<year_month_day>& holidays() const { return holidays_; }

	protected:
		std::vector<CashFlow> principalCashflows_;
		std::vector<CashFlow> interestCashflows_;

		double notional_ {0};
		year_month_day startDate_ {};
		year_month_day endDate_ {};
		EFrequency frequency_ {EFrequency::Annual};
		EStubType stubType_ {EStubType::ShortFirst};
		EDayCount dayCount_ {EDayCount::Actual_d365};
		EDateRule dateRule_ {EDateRule::ModFollowing};
		std::set<year_month_day> holidays_ {};
	};
}

#endif // __jetblack__rates__swap_leg_hpp
