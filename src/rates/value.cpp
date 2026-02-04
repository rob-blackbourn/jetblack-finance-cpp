#include "rates/value.hpp"
#include "rates/yield_curve.hpp"

#include <cmath>
#include <optional>
#include <ranges>
#include <stdexcept>

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	static double value(
		const year_month_day& valueDate,
		const YieldCurve& curve,
		const year_month_day& firstAccrualDate,
		const year_month_day& endDate,
		EDayCount dayCount,
		double rate,
		double notional)
	{
		double t = yearFrac(firstAccrualDate, endDate, dayCount);
		double df = curve.discountFactor(valueDate, endDate);
		double amount = notional * rate * t;
		double pv = amount * df;
		return pv;
	}

	static double value(
		const year_month_day& valueDate,
		const YieldCurve& curve,
		const year_month_day& endDate,
		EDayCount dayCount,
		double notional)
	{
		double df = curve.discountFactor(valueDate, endDate);
		double pv = notional * df;
		return pv;
	}

	double value(
		const year_month_day& valueDate,
		const YieldCurve& curve,
		const std::vector<year_month_day>& schedule,
		EDayCount dayCount,
		double rate,
		double notional)
	{
		double sum_pv = 0.0;

		for (
			auto &&[firstAccrualDate, endDate]
			: std::views::zip(schedule, schedule | std::views::drop(1)))
		{
			auto coupon_pv = value(valueDate, curve, firstAccrualDate, endDate, dayCount, rate, notional);
			sum_pv += coupon_pv;
		}

		auto notional_pv = value(valueDate, curve, schedule.back(), dayCount, notional);
		sum_pv += notional_pv;

		return sum_pv;
	}

	double value(
		const year_month_day& valueDate,
		const YieldCurve& curve,
		const std::vector<year_month_day>& schedule,
		EDayCount dayCount,
		const std::vector<double>& fixingRates,
		double notional)
	{
		double sum_pv = 0;

		for (
			auto &&[firstAccrualDate, endDate, rate]
			: std::views::zip(
				schedule,
				schedule | std::views::drop(1),
				fixingRates))
		{
			auto cashflow_pv = value(valueDate, curve, firstAccrualDate, endDate, dayCount, rate, notional);
			sum_pv += cashflow_pv;
		}

		auto notional_pv = value(valueDate, curve, schedule.back(), dayCount, notional);
		sum_pv += notional_pv;

		return sum_pv;
	}

	static double value(
		const year_month_day& valueDate,
		double yield,
		const year_month_day& firstAccrualDate,
		const year_month_day& endDate,
		EDayCount dayCount,
		double rate,
		double notional,
		EFrequency frequency)
	{
		double period_t = dates::yearFrac(firstAccrualDate, endDate, dayCount);
		double cashflow = rate * notional * period_t;
		double t = dates::yearFrac(valueDate, endDate, dayCount);
		double periods = t * static_cast<int>(frequency);
		double x = std::pow(1 + yield / std::to_underlying(frequency), periods);
		if (x == 0)
			throw std::runtime_error{"unable to calculate value - divide by zero"};
		double pv = cashflow / x;
		return pv;
	}

	static double value(
		const year_month_day& valueDate,
		double yield,
		const year_month_day& endDate,
		EDayCount dayCount,
		double notional,
		EFrequency frequency)
	{
		double t = dates::yearFrac(valueDate, endDate, dayCount);
		double periods = t * static_cast<int>(frequency);
		double x = std::pow(1 + yield / static_cast<int>(frequency), periods);
		if (x == 0)
			throw std::runtime_error{"unable to calculate value - divide by zero"};
		double pv = notional / x;
		return pv;
	}

	double value(
		const year_month_day& valueDate,
		double yield,
		const std::vector<year_month_day>& schedule,
		EDayCount dayCount,
		double rate,
		double notional,
		EFrequency frequency)
	{
		double sum_pv = 0;

		for (auto &&[firstAccrualDate, endDate] : std::views::zip(schedule, schedule | std::views::drop(1)))
		{
			auto coupon_pv = value(valueDate, yield, firstAccrualDate, endDate, dayCount, rate, notional, frequency);
			sum_pv += coupon_pv;
		}

		auto notional_pv = value(valueDate, yield, schedule.back(), dayCount, notional, frequency);
		sum_pv += notional_pv;

		return sum_pv;
	}
}