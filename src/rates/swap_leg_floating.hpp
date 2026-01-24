#ifndef __jetblack__rates__swap_leg_floating_hpp
#define __jetblack__rates__swap_leg_floating_hpp

#include <chrono>
#include <optional>
#include <set>
#include <utility>

#include "dates/schedules.hpp"
#include "dates/terms.hpp"
#include "dates/frequency.hpp"

#include "rates/swap_leg.hpp"

namespace rates
{
	using namespace std::chrono;
	using namespace rates;

	class YieldCurve;

	typedef std::pair< std::optional<double>, std::optional<double> > swap_fixing_t;

	class SwapLegFloating : public SwapLeg
	{
	public:

		SwapLegFloating() = default;

		SwapLegFloating(
			double notional,
			const year_month_day& startDate,
			const time_unit_t& tenor,
			EFrequency frequency,
			EStubType stubType,
			EDayCount dayCount,
			EDateRule dateRule,
			const std::set<year_month_day>& holidays,
			const time_unit_t& fixLag,
			double spread)
			:	SwapLeg(notional, startDate, tenor, frequency, stubType, dayCount, dateRule, holidays),
				fixLag_(fixLag),
				spread_(spread)
		{
			generate();
		}

		SwapLegFloating(
			double notional,
			const year_month_day& startDate,
			const year_month_day& endDate,
			EFrequency frequency,
			EStubType stubType,
			EDayCount dayCount,
			EDateRule dateRule,
			const std::set<year_month_day>& holidays,
			const time_unit_t& fixLag,
			double spread)
			:	SwapLeg(notional, startDate, endDate, frequency, stubType, dayCount, dateRule, holidays),
				fixLag_(fixLag),
				spread_(spread)
		{
			generate();
		}

		const std::vector<CashFlow>& principalCashlows() const;

		swap_fixing_t fixFloatingRates(const YieldCurve& curve, const swap_fixing_t& swap_fixing);

	private:

		void generate();

	private:
		time_unit_t fixLag_ {days{0}};
		double spread_ {0};
	};
}

#endif // __jetblack__rates__swap_leg_floating_hpp
