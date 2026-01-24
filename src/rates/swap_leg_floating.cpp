#include "rates/swap_leg_floating.hpp"
#include "rates/yield_curve.hpp"

#include "dates/schedules.hpp"

#include <ranges>

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	// Generate the floating leg flows - populate the data but don't calculate the amounts yet
	void SwapLegFloating::generate()
	{
		// Do the interest cashflows first
		auto dates = dates::generateSchedule(startDate_, endDate_, frequency_, stubType_, dateRule_, holidays_);

		if (dates.size() < 2)
			throw std::runtime_error{"SwapLegFloating::generate(year_month_day&,...) not enough dates in schedule"};

		auto start = dates.front();
		for (const auto& date : std::ranges::drop_view(dates, 1))
		{
			interestCashflows_.push_back(CashFlow(start, date, notional_, dayCount_));
			start = date;
		}

		// Now add in the principal at the end
		principalCashflows_.push_back(CashFlow(dates.front(), interestCashflows_.back().endDate(), notional_));
	}

	// Actually populate the cashflows with the amounts - from the fixes and from the yield curve.
	// fixFromCurve basically says if we need a fix, then look up the rate from the curve - we need this when using swaps to
	// build the yield curve. nextFix and thisFix are populated if they are looked up from the curve - so we can use them
	// when calculating DV01's.
	swap_fixing_t SwapLegFloating::fixFloatingRates(const YieldCurve& curve, const swap_fixing_t& swap_fixing)
	{
		std::optional<double> this_fixing, next_fixing;

		for (auto& cashflow : interestCashflows_)
		{
			auto fixDate = add(cashflow.startDate(),  -fixLag_, true, dateRule_, holidays_);

			double fix = 0.0;
			if (curve.valueDate() >= cashflow.startDate())
			{
				// Cashflow started in the past.

				if (!swap_fixing.first)
				{
					this_fixing = fix = curve.fix(curve.valueDate(), cashflow.endDate(), cashflow.dayCount());
				}
				else
				{
					this_fixing = fix = *swap_fixing.first;
				}
			}
			else if (curve.valueDate() >= fixDate)
			{
				// Cashflow started in the future, but the fix date is in the past.

				if (!swap_fixing.second)
				{
					next_fixing = fix = curve.fix(curve.valueDate(), cashflow.endDate(), cashflow.dayCount());
				}
				else
				{
					next_fixing = fix = *swap_fixing.second;
				}
			}
			else
			{
				// fix occurs in the future.

				fix = curve.fix(cashflow.startDate(), cashflow.endDate(), cashflow.dayCount());
			}

			cashflow.rate(fix + spread_);
		}

		return swap_fixing_t(this_fixing, next_fixing);
	}
}