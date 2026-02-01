#ifndef __jetblack__rates__ir_swap_leg_hpp
#define __jetblack__rates__ir_swap_leg_hpp

#include <chrono>
#include <set>
#include <vector>

#include "dates/schedules.hpp"
#include "dates/terms.hpp"
#include "dates/schedules.hpp"

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	class YieldCurve;

	class IrSwapLeg
	{
	protected:
		double notional_ {0};
		year_month_day startDate_ {};
		year_month_day maturity_ {};
		EFrequency frequency_ {EFrequency::Annual};
		EStubType stubType_ {EStubType::ShortFirst};
		EDayCount dayCount_ {EDayCount::Actual_d365};
		EDateRule dateRule_ {EDateRule::ModFollowing};
		std::vector<year_month_day> schedule_ {};
		
	public:
		IrSwapLeg() = default;

		IrSwapLeg(
			double notional,
			const year_month_day& startDate,
			const year_month_day& maturity,
			EFrequency frequency,
			EStubType stubType,
			EDayCount dayCount,
			EDateRule dateRule,
			const std::set<year_month_day>& holidays);
		
		IrSwapLeg(
			double notional,
			const year_month_day& startDate,
			const time_unit_t& tenor,
			EFrequency frequency,
			EStubType stubType,
			EDayCount dayCount,
			EDateRule dateRule,
			const std::set<year_month_day>& holidays);

		virtual double value(const year_month_day& valueDate, const YieldCurve& curve) const = 0;
		virtual double value(const YieldCurve& curve) const = 0;
		virtual double accrued(const YieldCurve& curve, const year_month_day& valueDate) const = 0;

		double notional() const { return notional_; }
		const year_month_day& startDate() const { return startDate_; }
		const year_month_day& maturity() const { return maturity_; }
		EFrequency frequency() const { return frequency_; }
		EStubType stubType() const { return stubType_; }
		EDayCount dayCount() const { return dayCount_; }
		EDateRule dateRule() const { return dateRule_; }
		const std::vector<year_month_day>& schedule() const { return schedule_; }
	};
}

#endif // __jetblack__rates__ir_swap_leg_hpp
