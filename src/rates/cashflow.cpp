#include "rates/cashflow.hpp"
#include "rates/yield_curve.hpp"

#include <ranges>
#include <stdexcept>
#include <type_traits>

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	/*
	 * Assumes here that the flow has been populated
	 */

	double CashFlow::value(const YieldCurve& curve) const
	{
		return value(curve.valueDate(), curve);
	}

	double CashFlow::value(const year_month_day& valueDate, const YieldCurve& curve) const
	{
		// If the cashflow has already occurred then zero value
		if (endDate_ <= valueDate)
			return 0.0;

		return flow_ * curve.discountFactor(valueDate, endDate_);
	}

	double CashFlow::value(const year_month_day& valueDate, double yield, EFrequency frequency) const
	{
		if (endDate_ <= valueDate)
			return 0.0;

		double t = yearFrac(valueDate, endDate_, dayCount_);
		double f = std::to_underlying(frequency);
		double periods = t * f;
		double tmp = pow(1 + yield / f, periods);
		if (tmp == 0.0)
			throw std::runtime_error("CashFlow::value(year_month_day,double,frequency) attempt to divide by zero");
		return flow_ / tmp;
	}

	double valueCashflows(const std::vector<CashFlow>& cashflows, const YieldCurve& curve)
	{
		double value = 0;
		for (const auto& cashflow : cashflows)
			value += cashflow.value(curve);
		return value;
	}

	double calculateAccrued(const std::chrono::year_month_day& valueDate,  std::vector<CashFlow>& cashflows)
	{
		if (valueDate <= cashflows.front().startDate() || valueDate >= cashflows.back().endDate())
			return 0.0;

		for (const auto& cashflow : std::ranges::drop_view(cashflows, 1))
			if (valueDate >= cashflow.startDate() && valueDate < cashflow.endDate())
				return cashflow.rate() * cashflow.notional() * yearFrac(cashflow.startDate(), valueDate, cashflow.dayCount());

		return 0.0;
	}
}