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
		const std::vector<YieldCurvePoint>& points,
		const year_month_day& valueDate,
		EDayCount dayCount,
		EInterpolationMethod interpolationMethod)
		:	valueDate_(valueDate),
			points_(points),
			dayCount_(dayCount),
			interpolationMethod_(interpolationMethod),
			interpolator_(createInterpolator(points, interpolationMethod))
	{
	}

	YieldCurve::YieldCurve(
		double flatRate,
		const year_month_day& valueDate,
		EDayCount dayCount)
		:	YieldCurve(
				{{1.0, flatRate}},
				valueDate,
				dayCount,
				EInterpolationMethod::Linear)
	{
	}

	YieldCurve::YieldCurve(
		const year_month_day& valueDate,
		const std::vector<std::shared_ptr<Instrument>>& instruments,
		EDayCount dayCount,
		EInterpolationMethod interpolationMethod)
		:	valueDate_(valueDate),
			instruments_(instruments),
			dayCount_(dayCount),
			interpolationMethod_(interpolationMethod)
	{
		buildCurve();
	}

	void YieldCurve::buildCurve()
	{
		if (instruments_.empty())
			throw std::length_error("instruments required for yield curve building");

		// Sort the instruments into chronological order
		std::ranges::sort(
			instruments_,
			[](const std::shared_ptr<Instrument>& a, const std::shared_ptr<Instrument>& b)
			{
				return a->endDate() < b->endDate();
			});

		solveZeroRates();
	}

	void YieldCurve::setLastRate(double z)
	{
		points_.back().rate(z);
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

		return YieldCurve(points, valueDate_, dayCount_);
	}

	YieldCurve YieldCurve::bumpInstruments(double x) const
	{
        auto instruments = instruments_
			| std::views::transform(
				[](auto&& instrument)
				{
					return instrument->clone_shared();
				})
			| std::ranges::to<std::vector<std::shared_ptr<Instrument>>>();

		for (auto&& instrument : instruments)
		{
			instrument->rate(instrument->rate() + x);
		}

		return YieldCurve(valueDate_, instruments, dayCount_, interpolationMethod_);
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

		double r = 0.05;

		for (auto&& instrument : instruments_)
		{
			auto t = time(instrument->endDate());
			points_.push_back({t, r});
			r = instrument->solveZeroRate(*this);
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
