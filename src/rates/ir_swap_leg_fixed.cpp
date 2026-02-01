#include "rates/accrued.hpp"
#include "rates/ir_swap_leg_fixed.hpp"
#include "rates/ir_swap.hpp"
#include "rates/value.hpp"
#include "rates/yield_curve.hpp"

#include <exception>

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	IrSwapLegFixed::IrSwapLegFixed(
		double notional,
		double rate,
		const year_month_day& startDate,
		const time_unit_t& tenor,
		EFrequency frequency,
		EStubType stubType,
		EDateRule dateRule,
		EDayCount dayCount,
		const std::set<year_month_day>& holidays)
		:	IrSwapLeg(notional, startDate, tenor, frequency, stubType, dayCount, dateRule, holidays),
			rate_(rate)
	{
	}

	IrSwapLegFixed::IrSwapLegFixed(
		double notional,
		double rate,
		const year_month_day& startDate,
		const year_month_day& maturity,
		EFrequency frequency,
		EStubType stubType,
		EDateRule dateRule,
		EDayCount dayCount,
		const std::set<year_month_day>& holidays)
		:	IrSwapLeg(notional, startDate, maturity, frequency, stubType, dayCount, dateRule, holidays),
			rate_(rate)
	{
	}

	double IrSwapLegFixed::accrued(const YieldCurve& curve, const year_month_day& valueDate) const
	{
		return rates::accrued(valueDate, schedule_, dayCount_, rate_, notional_);
	}

	double IrSwapLegFixed::value(const year_month_day& valueDate, const YieldCurve& curve) const
	{
		return rates::value(valueDate, curve, schedule_, dayCount_, rate_, notional_);
	}

	double IrSwapLegFixed::value(const YieldCurve& curve) const
	{
		return value(curve.valueDate(), curve);
	}

	double IrSwapLegFixed::calculateZeroRate(const YieldCurve& curve) const
	{
		double x = 1.0;
		double divisor;

		if (schedule_.size() == 2)
		{
			auto t = yearFrac(schedule_.front(), schedule_.back(), dayCount_);
			divisor = 1.0 + rate_ * t;
		}
		else
		{
			std::vector<year_month_day>::const_iterator i_date(schedule_.begin() + 1);
			while (i_date != schedule_.end() - 1)
			{
				auto t = yearFrac(*(i_date-1), *i_date, dayCount_);
				auto df = curve.discountFactor(*i_date);
				x = x - rate_ * df * t;
				i_date++;
			}

			auto t = yearFrac(*(i_date-1), *i_date, dayCount_);
			divisor = 1.0 + rate_ * t;
		}

		if (divisor == 0.0)
			throw std::runtime_error{"unable to calculate ir-swap zero rate - attempt to divide by zero"};

		auto t = curve.time(maturity_);
		if (t == 0)
			throw std::runtime_error{"unable to calculate ir-swap zero rate - swap has matured"};

		if (x <= 0.0)
			throw std::runtime_error{"unable to calculate ir-swap zero rate - attempt to log a non-positive number"};

		double r = x / divisor;

		return -1.0 / t * log(r);
	}
}