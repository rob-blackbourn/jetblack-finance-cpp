#ifndef __jetblack__rates__yield_curve_hpp
#define __jetblack__rates__yield_curve_hpp

#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "dates/terms.hpp"
#include "maths/interp.hpp"

#include "rates/deposit.hpp"
#include "rates/ir_future.hpp"
#include "rates/ir_swap.hpp"
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
	public:
		YieldCurve();

		YieldCurve(
			const std::string& label,
			const std::vector<YieldCurvePoint>& points,
			const year_month_day& valueDate,
			EDayCount dayCount,
			EInterpolationMethod interpolationMethod = EInterpolationMethod::Linear);
		
		YieldCurve(
			const std::string& label,
			double flatRate,
			const year_month_day& valueDate,
			EDayCount dayCount);
		
		YieldCurve(
			const std::string& label,
			const year_month_day& valueDate,
			const std::vector<Deposit>& deposits,
			const std::vector<IrFuture>& futures,
			const std::vector<IrSwap>& swaps,
			EDayCount dayCount,
			EInterpolationMethod interpolationMethod);

		const std::string& label() const { return label_; }
		const year_month_day& valueDate() const { return valueDate_; }
		const std::vector<YieldCurvePoint>& points() const { return points_; }
		EDayCount dayCount() const { return dayCount_; }

		bool operator == (const YieldCurve& rhs) const { return label_ == rhs.label_; }

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
		void deleteOverlappingFutures(const std::vector<Deposit>& deposits, std::vector<IrFuture>& futures);

		static std::shared_ptr<maths::Interp> createInterpolator(const std::vector<YieldCurvePoint>& points, EInterpolationMethod interpolationMethod);

	private:
		std::string						label_;
		year_month_day					valueDate_;
		std::vector<Deposit>			deposits_;
		std::vector<IrFuture>			futures_;
		std::vector<IrSwap>				swaps_;
		std::vector<YieldCurvePoint>	points_;
		EDayCount						dayCount_;
		EInterpolationMethod			interpolationMethod_;
		std::shared_ptr<maths::Interp>	interpolator_;
	};
}

extern rates::EInterpolationMethod& operator>>(const std::string& lhs, rates::EInterpolationMethod& rhs);
extern std::string& operator<<(std::string& lhs, const rates::EInterpolationMethod& rhs);
extern std::ostream& operator<<(std::ostream& lhs, const rates::EInterpolationMethod& rhs);

#endif // __jetblack__rates__yield_curve_hpp
