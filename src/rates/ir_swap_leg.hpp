#ifndef __jetblack__rates__ir_swap_leg_hpp
#define __jetblack__rates__ir_swap_leg_hpp

#include <chrono>
#include <set>
#include <vector>

#include "dates/schedules.hpp"
#include "dates/terms.hpp"
#include "dates/frequency.hpp"

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	class YieldCurve;

	class IrSwapLeg
	{
	public:
		IrSwapLeg() = default;

		IrSwapLeg(
			const year_month_day& startDate,
			const year_month_day& endDate,
			double notional,
			EFrequency frequency,
			EStubType stubType,
			EDayCount dayCount,
			EDateRule dateRule,
			const std::set<year_month_day>& holidays);
		
		IrSwapLeg(
			const year_month_day& startDate,
			const time_unit_t& tenor,
			double notional,
			EFrequency frequency,
			EStubType stubType,
			EDayCount dayCount,
			EDateRule dateRule,
			const std::set<year_month_day>& holidays);

		virtual double value(const year_month_day& valueDate, const YieldCurve& curve) const = 0;
		virtual double value(const YieldCurve& curve) const = 0;
		virtual double accrued(const year_month_day& valueDate) const = 0;

		const year_month_day& startDate() const { return startDate_; }
		const year_month_day& endDate() const { return endDate_; }
		double notional() const { return notional_; }
		EFrequency frequency() const { return frequency_; }
		EStubType stubType() const { return stubType_; }
		EDayCount dayCount() const { return dayCount_; }
		EDateRule dateRule() const { return dateRule_; }
		const std::vector<year_month_day>& schedule() const { return schedule_; }

	protected:
		year_month_day startDate_ {};
		year_month_day endDate_ {};
		double notional_ {0};
		EFrequency frequency_ {EFrequency::Annual};
		EStubType stubType_ {EStubType::ShortFirst};
		EDayCount dayCount_ {EDayCount::Actual_d365};
		EDateRule dateRule_ {EDateRule::ModFollowing};
		std::vector<year_month_day> schedule_ {};
	};
}

#endif // __jetblack__rates__ir_swap_leg_hpp
