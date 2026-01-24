#ifndef __jetblack__rates__swap_leg_fixed_hpp
#define __jetblack__rates__swap_leg_fixed_hpp

#include <chrono>
#include <set>
#include <vector>

#include "dates/schedules.hpp"
#include "dates/terms.hpp"

#include "rates/swap_leg.hpp"

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	class YieldCurve;

	/*
	 * Fixed swap leg. The amounts in the flows are calculated when the class is created.
	 */

	class SwapLegFixed : public SwapLeg
	{
	public:
		SwapLegFixed() = default;

		SwapLegFixed(
			double notional,
			const year_month_day& startDate,
			const time_unit_t& tenor,
			EFrequency frequency,
			EStubType stubType,
			EDayCount dayCount,
			EDateRule dateRule,
			const std::set<year_month_day>& holidays,
			double rate)
			:	SwapLeg(notional, startDate, tenor, frequency, stubType, dayCount, dateRule, holidays),
				rate_(rate)
		{
			generate();
		}

		SwapLegFixed(
			double notional,
			const year_month_day& startDate,
			const year_month_day& endDate,
			EFrequency frequency,
			EDayCount dayCount,
			EStubType stubType,
			EDateRule dateRule,
			const std::set<year_month_day>& holidays,
			double rate)
			:	SwapLeg(notional, startDate, endDate, frequency, stubType, dayCount, dateRule, holidays),
				rate_(rate)
		{
			generate();
		}

		double calculateZeroRate(const YieldCurve& curve) const;

		const year_month_day& endDate() const;
		double rate() const { return interestCashflows_.front().rate(); }
		void rate(double r);
		EDayCount dayCount() const { return interestCashflows_.front().dayCount(); }

	private:
		void generate();

	private:
		double rate_ {0};
	};
}

#endif // __jetblack__rates__swap_leg_fixed_hpp
