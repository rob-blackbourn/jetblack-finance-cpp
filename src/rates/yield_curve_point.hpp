#ifndef __jetblack__rates__yield_curve_point_hpp
#define __jetblack__rates__yield_curve_point_hpp

namespace rates
{
	class YieldCurvePoint
	{
	public:

		YieldCurvePoint() = default;

		YieldCurvePoint(double time, double rate)
			:	time_(time),
				rate_(rate)
		{
		}

		double rate() const { return rate_; }
		void rate(double r) { rate_ = r; }

		double time() const { return time_; }
		void time(double t) { time_ = t; }

		bool operator==(const YieldCurvePoint&) const = default;


	private:
		double time_ {}; // time in years as a double.
		double rate_ {};
	};
}

#endif // __jetblack__rates__yield_curve_point_hpp
