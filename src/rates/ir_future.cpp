#include "rates/ir_future.hpp"
#include "rates/yield_curve.hpp"

#include "maths/brent.hpp"

#include <limits>

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	IrFuture::IrFuture(
		double notional,
		double price,
		const year_month_day& expiryDate,
		const months& nMonths,
		EDayCount dayCount,
		EDateRule dateRule,
		const time_unit_t& spotLead,
		const std::set<year_month_day>& holidays)
	{
		year_month_day startDate = add(expiryDate, spotLead, false, dateRule, holidays);
		// The price is quoted as a discount from par. e.g. 0.05 is 95.
		double rate = (100 - price) / 100.0;
		deposit_ = Deposit(notional, rate, startDate, nMonths, dayCount, dateRule, holidays);
	}

	// A regular interest rate future is a just a deposit starting spot days
	// after expiry and lasting three months.
	IrFuture::IrFuture(
		double notional,
		double price,
		const year_month& expiry,
		EDayCount dayCount,
		EDateRule dateRule,
		const time_unit_t& spotLead,
		const std::set<year_month_day>& holidays)
		: IrFuture(
			notional,
			price,
			immDate(expiry),
			months{3},
			dayCount,
			dateRule,
			spotLead,
			holidays)
	{
	}

	double IrFuture::value(const YieldCurve& curve) const
	{
		return deposit_.value(curve);
	}

	double IrFuture::calculateZeroRate(const YieldCurve& curve) const
	{
		return deposit_.calculateZeroRate(curve);
	}

	double IrFuture::solveZeroRate(YieldCurve& curve, size_t index)
	{
		const double ERROR_TOLERANCE = std::numeric_limits<double>::epsilon();
		const unsigned int MAX_ITERATIONS = 30;

		return maths::brent::solve(
			[&curve, index, this](double rate)
			{
				curve.rate(index, rate);
				return this->value(curve);
			},
			-0.1, 1.0, MAX_ITERATIONS, ERROR_TOLERANCE
		);		
	}
}