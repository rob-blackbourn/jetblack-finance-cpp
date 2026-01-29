#include "rates/deposit.hpp"
#include "rates/yield_curve.hpp"

#include "NR/root_secant.hpp"
#include "maths/brent.hpp"

#include <limits>

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	Deposit::Deposit(
		double notional,
		double rate,
		const year_month_day& startDate,
		const year_month_day& endDate,
		EDayCount dayCount)
		:	notional_(notional),
			rate_(rate),
			startDate_(startDate),
			endDate_(endDate),
			dayCount_(dayCount)
	{
	}

	Deposit::Deposit(
		double notional,
		double rate,
		const year_month_day& startDate,
		const time_unit_t& tenor,
		EDayCount dayCount,
		EDateRule dateRule,
		const std::set<year_month_day>& holidays)
		:	Deposit(
				notional,
				rate,
				startDate,
				add(startDate, tenor, isEndOfMonth(startDate), dateRule, holidays),
				dayCount)
	{
	}

	double Deposit::value(const YieldCurve& curve) const
	{
		// We assume we deposit $1 on the start date and receive back $1 plus interest on the end date.
		double dfStart = curve.discountFactor(startDate_);
		double dfEnd = curve.discountFactor(endDate_);
		double t = yearFrac(startDate_, endDate_, dayCount_);

		double interest = notional_ * rate_ * t;
		double endCashFlow = notional_ + interest;
		double npv = endCashFlow * dfEnd - notional_ * dfStart;

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

	double Deposit::solveZeroRate(YieldCurve& curve, size_t index)
	{
		const double ERROR_TOLERANCE = std::numeric_limits<double>::epsilon();
		const unsigned int MAX_ITERATIONS = 100;

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