#include "rates/value.hpp"
#include "rates/yield_curve.hpp"

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
		const year_month_day& startDate,
		const year_month_day& endDate,
		EDayCount dayCount,
		double rate,
		double notional)
	{
		double t = yearFrac(startDate, endDate, dayCount);
		double df = curve.discountFactor(valueDate, endDate);
		double amount = notional * rate * t;
		double pv = amount * df;
		return pv;
	}

	static double value(
		const year_month_day& valueDate,
		const YieldCurve& curve,
		const year_month_day& startDate,
		const year_month_day& endDate,
		EDayCount dayCount,
		const std::optional<double>& fixingRate,
		double notional)
	{
		if (!fixingRate)
			throw std::domain_error("rate not fixed");
		return value(valueDate, curve, startDate, endDate, dayCount, *fixingRate, notional);
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
		double couponRate,
		double notional)
	{
		double sum_pv = 0.0;

		for (const auto &[startDate, endDate] : std::views::zip(schedule, schedule | std::views::drop(1)))
		{
			auto coupon_pv = value(valueDate, curve, startDate, endDate, dayCount, couponRate, notional);
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
		const std::vector<double>& fixings,
		double notional)
	{
		double pv, sum_pv = 0;

		for (const auto &[startDate, endDate, fixing] : std::views::zip(schedule, schedule | std::views::drop(1), fixings))
		{
			pv = value(valueDate, curve, startDate, endDate, dayCount, fixing, notional);
			sum_pv += pv;
		}

		pv = value(valueDate, curve, schedule.back(), dayCount, notional);
		sum_pv += pv;

		return sum_pv;
	}

	double value(
		const year_month_day& valueDate,
		const YieldCurve& curve,
		const std::vector<year_month_day>& schedule,
		EDayCount dayCount,
		const std::vector<Fixing>& fixings,
		double notional)
	{
		double sum_pv = 0;

		for (const auto &[startDate, endDate, fixing] : std::views::zip(schedule, schedule | std::views::drop(1), fixings))
		{
			auto cashflow_pv = value(valueDate, curve, startDate, endDate, dayCount, fixing.rate(), notional);
			sum_pv += cashflow_pv;
		}

		auto notional_pv = value(valueDate, curve, schedule.back(), dayCount, notional);
		sum_pv += notional_pv;

		return sum_pv;
	}

	static double value(
		const year_month_day& valueDate,
		double yield,
		const year_month_day& startDate,
		const year_month_day& endDate,
		EDayCount dayCount,
		double couponRate,
		double notional,
		EFrequency frequency)
	{
		double period_t = dates::getTerm(startDate, endDate, dayCount).second;
		double amount = couponRate * notional * period_t;
		double t = dates::getTerm(valueDate, endDate, dayCount).second;
		double periods = t * static_cast<int>(frequency);
		double x = ::pow(1 + yield / static_cast<int>(frequency), periods);
		if (x == 0)
			throw std::runtime_error{"unable to calculate value - divide by zero"};
		double pv = amount / x;
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
		double t = dates::getTerm(valueDate, endDate, dayCount).second;
		double periods = t * static_cast<int>(frequency);
		double x = ::pow(1 + yield / static_cast<int>(frequency), periods);
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
		double couponRate,
		double notional,
		EFrequency frequency)
	{
		double sum_pv = 0;

		for (const auto &[startDate, endDate] : std::views::zip(schedule, schedule | std::views::drop(1)))
		{
			auto coupon_pv = value(valueDate, yield, startDate, endDate, dayCount, couponRate, notional, frequency);
			sum_pv += coupon_pv;
		}

		auto notional_pv = value(valueDate, yield, schedule.back(), dayCount, notional, frequency);
		sum_pv += notional_pv;

		return sum_pv;
	}
}