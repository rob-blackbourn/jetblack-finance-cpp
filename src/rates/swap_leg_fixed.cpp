#include "rates/swap_leg_fixed.hpp"
#include "rates/yield_curve.hpp"

#include "dates/schedules.hpp"

#include <ranges>

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	void SwapLegFixed::rate(double r)
	{
		for (auto& cashflow : interestCashflows_)
			cashflow.rate(r);
	}

	// Generate the fixed leg flows - we actually populate the amounts of the flows

	void SwapLegFixed::generate()
	{
		auto dates = dates::generateSchedule(startDate_, endDate_, frequency_, stubType_, dateRule_, holidays_);
		if (dates.size() < 2)
		throw std::runtime_error{"swap_fixed_leg_t::generate(year_month_day,...) not enough dates in schedule"};

		auto start = dates.front();
		for (const auto& date : std::ranges::drop_view(dates, 1))
		{
			const auto& end = date;
			interestCashflows_.push_back(CashFlow(start, end, notional_, dayCount_, rate_));
			start = end;
		}

		// Add in the principal cashflow
		principalCashflows_.push_back(CashFlow(dates.front(), interestCashflows_.back().endDate(), notional_));
	}

	const year_month_day& SwapLegFixed::endDate() const
	{
		if (interestCashflows_.empty() && principalCashflows_.empty())
			throw "no cashflows exist";
		else if (interestCashflows_.empty() || principalCashflows_.back().endDate() > interestCashflows_.back().endDate())
			return principalCashflows_.back().endDate();
		else
			return interestCashflows_.back().endDate();
	}

	double SwapLegFixed::calculateZeroRate(const YieldCurve& curve) const
	{
		
		double x = 1.0;
		double t;
		double divisor;

		if (interestCashflows_.empty())
		{
			throw "cashflows are empty";
		}
		else if (interestCashflows_.size() == 1)
		{
			// What if only one cashflow?
			t = yearFrac(interestCashflows_.front().startDate(), interestCashflows_.front().endDate(), interestCashflows_.front().dayCount());
			divisor = 1.0 + rate() * t;
		}
		else
		{
			std::vector<CashFlow>::const_iterator i_cashflow = interestCashflows_.begin();
			while (i_cashflow != interestCashflows_.end() - 1)
			{
				t = yearFrac(i_cashflow->startDate(), i_cashflow->endDate(), i_cashflow->dayCount());
				x = x - rate() * t * curve.discountFactor(i_cashflow->endDate());
				++i_cashflow;
			}

			t = yearFrac(i_cashflow->startDate(), i_cashflow->endDate(), i_cashflow->dayCount());
			divisor = 1.0 + rate() * t;
		}

		if (divisor == 0.0)
		throw std::runtime_error{"swap_fixed_leg_t::calculateZeroRate(YieldCurve*) attempt to divide by zero"};

		t = curve.calculateTime(endDate());

		if (t == 0)
			throw std::runtime_error{"swap_fixed_leg_t::calculateZeroRate(YieldCurve*) attempt to divide by zero"};

		if (x <= 0.0)
			throw std::runtime_error{"swap_fixed_leg_t::calculateZeroRate(YieldCurve*) attempt to log a non-positive number"};
		x = x / divisor;
		return -1.0 / t * log(x);
	}
}