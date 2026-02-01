#ifndef __jetblack__rates__yield_curve_hpp
#define __jetblack__rates__yield_curve_hpp

#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "dates/terms.hpp"
#include "maths/interp.hpp"

#include "rates/instrument.hpp"
#include "rates/yield_curve_point.hpp"

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	enum class EInterpolationMethod
	{
		Linear,
		CubicSpline,
		FlatForward,
		Hermite,
		Exponential
	};

	class YieldCurve
	{
	private:
		year_month_day					valueDate_;
		std::vector<std::shared_ptr<Instrument>>	instruments_;
		std::vector<YieldCurvePoint>	points_;
		EDayCount						dayCount_;
		EInterpolationMethod			interpolationMethod_;
		std::shared_ptr<maths::Interp>	interpolator_;

	public:
		YieldCurve();

		YieldCurve(
			const std::vector<YieldCurvePoint>& points,
			const year_month_day& valueDate,
			EDayCount dayCount,
			EInterpolationMethod interpolationMethod = EInterpolationMethod::Linear);
		
		YieldCurve(
			double flatRate,
			const year_month_day& valueDate,
			EDayCount dayCount);
		
		YieldCurve(
			const year_month_day& valueDate,
			const std::vector<std::shared_ptr<Instrument>>& instruments,
			EDayCount dayCount,
			EInterpolationMethod interpolationMethod);

		const year_month_day& valueDate() const { return valueDate_; }
		const std::vector<YieldCurvePoint>& points() const { return points_; }
		EDayCount dayCount() const { return dayCount_; }

		YieldCurve shift(double) const;
		YieldCurve bumpInstruments(double) const;

		double rate(double t) const;
		double rate(const year_month_day& date) const;
		void rate(size_t i, double z);

		double forwardRate(double t1, double t2) const;
		double forwardRate(const year_month_day& startDate, const year_month_day& endDate) const;

		double discountFactor(double t) const;
		double discountFactor(const year_month_day& date) const;
		double discountFactor(double t1, double t2) const;
		double discountFactor(const year_month_day& startDate, const year_month_day& endDate) const;

		double fix(const year_month_day& valueDate, const year_month_day& fixingDate, EDayCount dayCount) const;

		double time(const year_month_day& date) const;

	private:
		void buildCurve();
		void solveZeroRates();

		static std::shared_ptr<maths::Interp> createInterpolator(const std::vector<YieldCurvePoint>& points, EInterpolationMethod interpolationMethod);
	};
}

extern rates::EInterpolationMethod& operator>>(const std::string& lhs, rates::EInterpolationMethod& rhs);
extern std::string& operator<<(std::string& lhs, const rates::EInterpolationMethod& rhs);
extern std::ostream& operator<<(std::ostream& lhs, const rates::EInterpolationMethod& rhs);

#endif // __jetblack__rates__yield_curve_hpp
