#include "rates/ir_future.hpp"
#include "rates/yield_curve.hpp"

#include "maths/brent.hpp"

#include <limits>

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	IrFuture::IrFuture(
		const year_month_day& expiryDate,
		const months& m,
		EDayCount dayCount,
		double price,
		EDateRule dateRule,
		const std::set<year_month_day>& holidays,
		const time_unit_t& spot)
	{
		// Interest rate future is a just a deposit starting spot days after expiry and lasting three months
		year_month_day startDate = add(expiryDate, spot, false, dateRule, holidays);
		double rate = (100 - price) / 100.0;
		deposit_ = Deposit(startDate, m, dayCount, rate, dateRule, holidays);
	}

	IrFuture::IrFuture(
		const year_month& expiry,
		EDayCount dayCount,
		double price,
		EDateRule dateRule,
		const std::set<year_month_day>& holidays,
		const time_unit_t& spot)
		: IrFuture(
			immDate(expiry),
			months{3},
			dayCount,
			price,
			dateRule,
			holidays,
			spot)
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

	double IrFuture::solveZeroRate(YieldCurve& curve, size_t index) const
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