#include "rates/accrued.hpp"
#include "rates/bond.hpp"
#include "rates/value.hpp"
#include "rates/yield_curve.hpp"

#include "dates/schedules.hpp"
#include "NR/bisector.hpp"

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	Bond::Bond(
		const year_month_day& firstAccrualDate,
		const year_month_day& maturityDate,
		double couponRate,
		EFrequency couponFrequency,
		EDayCount dayCount,
		EStubType stubType,
		double faceValue,
		EDateRule dateRule,
		const std::set<year_month_day>& holidays)
		:	Bond(
			generateSchedule(firstAccrualDate, maturityDate, couponFrequency, stubType, dateRule, holidays),
			firstAccrualDate,
			maturityDate,
			couponRate,
			couponFrequency,
			dayCount,
			stubType,
			faceValue,
			dateRule)
	{
		if (schedule_.size() < 2) throw "not enough dates in schedule";
	}

	double Bond::accruedInterest(const year_month_day& valueDate) const
	{
		return rates::accrued(valueDate, schedule_, dayCount_, couponRate_, faceValue_);
	}

	double Bond::value(const year_month_day& valueDate, const YieldCurve& curve) const
	{
		return rates::value(valueDate, curve, schedule_, dayCount_, couponRate_, faceValue_);
	}

	double Bond::value(const YieldCurve& curve) const
	{
		return value(curve.valueDate(), curve);
	}

	double Bond::value(const year_month_day& valueDate, double yield) const
	{
		return rates::value(valueDate, yield, schedule_, dayCount_, couponRate_, faceValue_, couponFrequency_);
	}

	double Bond::yield(const year_month_day& valueDate, double price) const
	{
		const double ERROR_TOLERANCE = 1e-11;
		const unsigned int MAX_ITERATIONS = 50;

		Bond bond { *this };

		return NR::bisector::solve(
			[&bond, price, &valueDate](double yield)
			{
				return price - bond.value(valueDate, yield);
			},
			-0.1, 2.0, MAX_ITERATIONS, ERROR_TOLERANCE
		);
	}
}