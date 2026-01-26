#include "rates/deposit.hpp"
#include "rates/yield_curve.hpp"

#include "NR/root_secant.hpp"
#include "maths/brent.hpp"

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	Deposit::Deposit(
		const year_month_day& startDate,
		const year_month_day& endDate,
		EDayCount dayCount,
		double rate)
		:	startDate_(startDate),
			endDate_(endDate),
			dayCount_(dayCount),
			rate_(rate)
	{
	}

	Deposit::Deposit(
		const year_month_day& startDate,
		const time_unit_t& tenor,
		EDayCount dayCount,
		double rate,
		EDateRule dateRule,
		const std::set<year_month_day>& holidays)
		:	Deposit(
				startDate,
				add(startDate, tenor, isEndOfMonth(startDate), dateRule, holidays),
				dayCount,
				rate)
	{
	}

	double Deposit::value(const YieldCurve& curve) const
	{
		// We assume we deposit $1 on the start date and receive back $1 plus interest on the end date.
		double dfStart = curve.discountFactor(startDate_);
		double dfEnd = curve.discountFactor(endDate_);
		double t = yearFrac(startDate_, endDate_, dayCount_);

		double notional = 1.0;
		double interest = notional * rate_ * t;
		double endCashFlow = notional + interest;
		double npv = endCashFlow * dfEnd - notional * dfStart;

		return npv;
	}

	double Deposit::calculateZeroRate(const YieldCurve& curve) const
	{
		double df = curve.discountFactor(startDate_);
		double payment = 1.0 + rate_ * yearFrac(startDate_, endDate_, dayCount_);

		if (df / payment <= 0.0)
			throw "unable to calculate zero rate for future - log of non-positive number";

		double t = curve.time(endDate_);
		double r = -1.0 / t * log(df / payment);

		return r;
	}

	double Deposit::solveZeroRate(YieldCurve& curve, size_t index) const
	{
		const double ERROR_TOLERANCE = 1e-15;
		const unsigned int MAX_ITERATIONS = 30;

		return maths::brent::solve(
			[&](double rate)
			{
				curve.rate(index, rate);
				return this->value(curve);
			},
			-0.1, 1.0, MAX_ITERATIONS, ERROR_TOLERANCE
		);
	}
}