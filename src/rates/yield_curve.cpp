#include <algorithm>
#include <cstring>
#include <exception>
#include <ranges>

#include "rates/yield_curve.hpp"

#include "maths/exp_interp.hpp"
#include "maths/flatfwd_interp.hpp"
#include "maths/hermite_interp.hpp"
#include "maths/linear_interp.hpp"
#include "maths/spline_interp.hpp"

#include "dates/terms.hpp"

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	YieldCurve::YieldCurve()
	{
	}
	
	YieldCurve::YieldCurve(
		const std::string& label,
		const std::vector<YieldCurvePoint>& points,
		const year_month_day& valueDate,
		EDayCount dayCount,
		EInterpolationMethod interpolationMethod)
		:	label_(label),
			valueDate_(valueDate),
			points_(points),
			dayCount_(dayCount),
			interpolationMethod_(interpolationMethod),
			interpolator_(createInterpolator(points, interpolationMethod))
	{
	}

	YieldCurve::YieldCurve(
		const std::string& label,
		double flatRate,
		const year_month_day& valueDate,
		EDayCount dayCount)
		:	YieldCurve(
				label,
				{{1.0, flatRate}},
				valueDate,
				dayCount,
				EInterpolationMethod::Linear)
	{
	}

	YieldCurve::YieldCurve(
		const std::string& label,
		const year_month_day& valueDate,
		const std::vector<Deposit>& deposits,
		const std::vector<IrFuture>& futures,
		const std::vector<IrSwap>& swaps,
		EDayCount dayCount,
		EInterpolationMethod interpolationMethod)
		:	label_(label),
			valueDate_(valueDate),
			deposits_(deposits),
			futures_(futures),
			swaps_(swaps),
			dayCount_(dayCount),
			interpolationMethod_(interpolationMethod)
	{
		buildCurve();
	}

	void YieldCurve::buildCurve()
	{
		if (deposits_.empty())
			throw std::length_error("deposits required for yield curve building");
		if (swaps_.empty())
			throw std::length_error("swaps required for yield curve building");

		// Sort the instruments into chronological order
		std::ranges::sort(
			deposits_,
			[](const Deposit& a, const Deposit& b)
			{
				return a.endDate() < b.endDate();
			});

		if (!futures_.empty())
		{
			std::ranges::sort(
				futures_,
				[](const IrFuture& a, IrFuture& b)
				{
					return a.endDate() < b.endDate();
				});
		}

		std::ranges::sort(
			swaps_,
			[](const IrSwap& a, const IrSwap& b)
			{
				return a.endDate() < b.endDate();
			});

		// Get rid of any futures which will put in a point before the end of the deposits
		deleteOverlappingFutures(deposits_, futures_);

		solveZeroRates();
	}

	void YieldCurve::rate(size_t i, double z)
	{
		points_[i].rate(z);
		interpolator_ = createInterpolator(
			points_,
			points_.size() == 1 ? EInterpolationMethod::Linear : interpolationMethod_
		);
	}

	double YieldCurve::rate(double t) const
	{
		if (t < 0.0)
			throw std::range_error("time is prior to value date");

		if (points_.size() == 0)
			throw std::range_error("no points in curve");

		return interpolator_->interpolate(t);
	}

	double YieldCurve::forwardRate(double t1, double t2) const
	{
		if (points_.size() == 1)
			return points_.front().rate();

		if (t1 == t2) return 0.0;

		double r1 = rate(t1);
		double r2 = rate(t2);

		return (r2 * t2 - r1 * t1) / (t2 - t1);
	}

	double YieldCurve::discountFactor(double t) const
	{
		return exp(-rate(t) * t);
	}

	double YieldCurve::discountFactor(double t1, double t2) const
	{
		return discountFactor(t2) / discountFactor(t1);
	}

	double YieldCurve::rate(const year_month_day& date) const
	{
		return rate(time(date));
	}

	double YieldCurve::forwardRate(const year_month_day& d1, const year_month_day& d2) const
	{
		return forwardRate(time(d1), time(d2));
	}

	double YieldCurve::discountFactor(const year_month_day& date) const
	{
		return discountFactor(time(date));
	}

	double YieldCurve::discountFactor(const year_month_day& d1, const year_month_day& d2) const
	{
		return discountFactor(time(d1), time(d2));
	}

	double YieldCurve::fix(const year_month_day& startDate, const year_month_day& endDate, EDayCount dayCount) const
	{
		if (startDate == endDate)
			return 0.0;

		double t = time(endDate) - time(startDate);
		double r = forwardRate(startDate, endDate);
		double period_t = yearFrac(startDate, endDate, dayCount);

		return (exp(r * t) - 1.0) / period_t;
	}

	YieldCurve YieldCurve::shift(double x) const
	{
		std::vector<YieldCurvePoint> points(points_);
		for (auto& point : points)
			point.rate(point.rate() + x);

		return YieldCurve(label_ + "_shifted", points, valueDate_, dayCount_);
	}

	YieldCurve YieldCurve::bumpInstruments(double x) const
	{
		std::vector<Deposit> deposits(deposits_);
		for (auto& deposit : deposits)
			deposit.rate(deposit.rate() + x);

		std::vector<IrFuture> futures(futures_);
		for (auto& future : futures)
			future.rate(future.rate() + x);

		std::vector<IrSwap> swaps(swaps_);
		for (auto& swap : swaps)
			swap.fixedLeg().rate(swap.fixedLeg().rate() + x);

		return YieldCurve(label_ + "_bumped", valueDate_, deposits, futures, swaps, dayCount_, interpolationMethod_);
	}

	double YieldCurve::time(const year_month_day& date) const
	{
		return yearFrac(valueDate_, date, dayCount_);
	}

	// Solver method - stick in a point for the end point of each instrument, and then solve for the zero rate at that point
	// which gives us a zero value for the instrument.
	void YieldCurve::solveZeroRates()
	{
		points_.clear();

		size_t i = 0;
		double r = 0.05;

		for (auto& deposit : deposits_)
		{
			auto t = time(deposit.endDate());
			points_.push_back({t, r});
			r = deposit.solveZeroRate(*this, i);
			rate(i++, r);
		}

		for (auto& future : futures_)
		{
			auto t = time(future.endDate());
			points_.push_back({t, r});
			r = future.solveZeroRate(*this, i);
			rate(i++, r);
		}

		for (auto& swap : swaps_)
		{
			auto t = time(swap.endDate());
			points_.push_back({t, r});
			r = swap.solveZeroRate(*this, i);
			rate(i++, r);
		}
	}

	std::shared_ptr<maths::Interp> YieldCurve::createInterpolator(const std::vector<YieldCurvePoint>& points, EInterpolationMethod interpolationMethod)
	{
		std::vector<double> x, y;

		for (const auto& point: points)
		{
			x.push_back(point.time());
			y.push_back(point.rate());
		}

		bool force_interpolation = true;
		bool extrapolate_near_flat = true;
		bool extrapolate_far_flat = false;

		if (points.size() < 2)
			return std::make_shared<maths::LinearInterp>(x, y, force_interpolation, extrapolate_near_flat, extrapolate_far_flat);

		switch (interpolationMethod)
		{
		case EInterpolationMethod::Linear:
			return std::make_shared<maths::LinearInterp>(x, y, force_interpolation, extrapolate_near_flat, extrapolate_far_flat);

		case EInterpolationMethod::FlatForward:
			return std::make_shared<maths::FlatForwardInterp>(x,y, force_interpolation, extrapolate_near_flat, extrapolate_far_flat);

		case EInterpolationMethod::CubicSpline:
			return std::make_shared<maths::SplineIterp>(x, y, force_interpolation, extrapolate_near_flat, extrapolate_far_flat, 0, 0);

		case EInterpolationMethod::Hermite:
			return std::make_shared<maths::HermiteInterp>(x, y, force_interpolation, extrapolate_near_flat, extrapolate_far_flat);

		case EInterpolationMethod::Exponential:
			return std::make_shared<maths::ExpInterp>(x, y, force_interpolation, extrapolate_near_flat, extrapolate_far_flat);
		
		default:
			throw std::runtime_error("Invalid interpolation method");
		}
	}

	void YieldCurve::deleteOverlappingFutures(const std::vector<Deposit>& deposits, std::vector<IrFuture>& futures)
	{	
		year_month_day depositsEndDate = deposits_.back().endDate();
		bool deletedFuture;
		do
		{
			deletedFuture = false;
			for (std::vector<IrFuture>::iterator i_future(futures.begin()); i_future != futures.end(); ++i_future)
			{
				if (i_future->endDate() <= depositsEndDate)
				{
					futures.erase(i_future);
					deletedFuture = true;
					break;
				}
			}
		}
		while (deletedFuture == true);
	}
}

static const struct { const char* string_type; rates::EInterpolationMethod enum_type; } InterpolationMethod_TypeMap[] =
{
	{ "Linear",	rates::EInterpolationMethod::Linear},

	{ "Flat Forward",	rates::EInterpolationMethod::FlatForward},
	{ "FlatForward",	rates::EInterpolationMethod::FlatForward},

	{ "Cubic Spline",	rates::EInterpolationMethod::CubicSpline},
	{ "CubicSpline",	rates::EInterpolationMethod::CubicSpline},

	{ "Hermite",		rates::EInterpolationMethod::Hermite},

	{ "Exponential",	rates::EInterpolationMethod::Exponential},
};

rates::EInterpolationMethod& operator>>(const std::string& lhs, rates::EInterpolationMethod& rhs)
{
	for (size_t i = 0; i < sizeof(InterpolationMethod_TypeMap) / sizeof(InterpolationMethod_TypeMap[0]); ++i)
		if (strcmp(lhs.c_str(), InterpolationMethod_TypeMap[i].string_type) == 0)
			return rhs = InterpolationMethod_TypeMap[i].enum_type;

	throw std::invalid_argument("unknown enum");
}

std::string& operator<<(std::string& lhs, const rates::EInterpolationMethod& rhs)
{
	for (size_t i = 0; i < sizeof(InterpolationMethod_TypeMap) / sizeof(InterpolationMethod_TypeMap[0]); ++i)
		if (rhs == InterpolationMethod_TypeMap[i].enum_type)
			return lhs = InterpolationMethod_TypeMap[i].string_type;

	throw std::invalid_argument("unknown enum");
}

std::ostream& operator<<(std::ostream& lhs, const rates::EInterpolationMethod& rhs)
{
	std::string s;
	s << rhs;
	lhs << s;
	return lhs;
}
