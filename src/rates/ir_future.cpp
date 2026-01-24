#include "rates/ir_future.hpp"
#include "rates/yield_curve.hpp"

#include "maths/brent.hpp"

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	IrFuture::IrFuture(
		const year_month_day& expiryDate,
		EDayCount dayCount,
		double price,
		double convexity,
		EDateRule dateRule,
		const std::set<year_month_day>& holidays,
		const time_unit_t& spot)
	{
		// Interest rate future is a just a deposit starting spot days after expiry and lasting three months
		year_month_day startDate = add(expiryDate, spot, false, dateRule, holidays);
		double rate = (100 - (price + convexity)) / 100.0;
		deposit_ = Deposit(startDate, months{3}, dayCount, rate, dateRule, holidays);
	}

	double IrFuture::value(const YieldCurve& curve) const
	{
		return deposit_.value(curve);
	}

	double IrFuture::calculateZeroRate(const YieldCurve& curve) const
	{
		return deposit_.calculateZeroRate(curve);
	}

	double IrFuture::solveZeroRate(const YieldCurve& curve, size_t index) const
	{
		const double ERROR_TOLERANCE = 1e-11;
		const unsigned int MAX_ITERATIONS = 30;

		YieldCurve crv(curve);

		return maths::brent::solve(
			[&crv, index, this](double rate)
			{
				crv.rate(index, rate);
				return this->value(crv);
			},
			-0.1, 1.0, MAX_ITERATIONS, ERROR_TOLERANCE
		);		
	}

	bool compareFutures(const IrFuture& f1, const IrFuture& f2)
	{
		return f1.endDate() < f2.endDate();
	}
}