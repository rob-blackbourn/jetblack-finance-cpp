#ifndef __jetblack__rates__ir_swap_leg_fixed_hpp
#define __jetblack__rates__ir_swap_leg_fixed_hpp

#include <chrono>
#include <set>

#include "rates/ir_swap_leg.hpp"

#include "dates/schedules.hpp"
#include "dates/terms.hpp"

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	class YieldCurve;

	class IrSwapLegFixed : public IrSwapLeg
	{
	public:
		IrSwapLegFixed() = default;

		IrSwapLegFixed(
			const year_month_day& startDate,
			const time_unit_t& tenor,
			EFrequency frequency,
			EStubType stubType,
			EDateRule dateRule,
			const std::set<year_month_day>& holidays,
			EDayCount dayCount,
			double notional,
			double rate);

		IrSwapLegFixed(
			const year_month_day& startDate,
			const year_month_day& endDate,
			EFrequency frequency,
			EStubType stubType,
			EDateRule dateRule,
			const std::set<year_month_day>& holidays,
			EDayCount dayCount,
			double notional,
			double rate);

		virtual double value(const year_month_day& valueDate, const YieldCurve& curve) const;
		virtual double value(const YieldCurve& curve) const;
		virtual double accrued(const year_month_day& valueDate) const;

		double calculateZeroRate(const YieldCurve& curve) const;

		double rate() const { return rate_; }
		void rate(double rate) { rate_ = rate; }

	private:
		double rate_ {0};
	};
}

#endif // __jetblack__rates__ir_swap_leg_fixed_hpp
