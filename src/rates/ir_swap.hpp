#ifndef __jetblack__rates__ir_swap_hpp
#define __jetblack__rates__ir_swap_hpp

#include <chrono>
#include <optional>

#include "rates/ir_swap_leg_fixed.hpp"
#include "rates/ir_swap_leg_floating.hpp"
#include "rates/ir_swap.hpp"

// Untested version

namespace rates
{
	using namespace std::chrono;

	class YieldCurve;

	class IrSwap
	{
	public:
		IrSwap() = default;

		IrSwap(
			const IrSwapLegFixed& fixedLeg,
			const IrSwapLegFloating& floatingLeg);

		IrSwap(
			const year_month_day& startDate,
			const time_unit_t& tenor,
			EFrequency frequency,
			EStubType stubType,
			EDateRule dateRule,
			EDayCount dayCount,
			double notional,
			double fixedRate,
			const time_unit_t& fixLag,
			double spread,
			const std::set<year_month_day>& holidays);

		IrSwap(
			const year_month_day& startDate,
			const year_month_day& endDate,
			EFrequency frequency,
			EStubType stubType,
			EDateRule dateRule,
			EDayCount dayCount,
			double notional,
			double fixedRate,
			const time_unit_t& fixLag,
			double spread,
			const std::set<year_month_day>& holidays);

		double value(const YieldCurve& curve) const;
		double value(const year_month_day& valueDate, const YieldCurve& curve) const;

		IrSwapLegFixed& fixedLeg() { return fixedLeg_; }
		const IrSwapLegFixed& fixedLeg() const { return fixedLeg_; }

		IrSwapLegFloating& floatingLeg() { return floatingLeg_; }
		const IrSwapLegFloating& floatingLeg() const { return floatingLeg_; }

		const year_month_day& endDate() const { return fixedLeg_.endDate(); }

		double calculateZeroRate(const YieldCurve& curve) const;
		double solveZeroRate(YieldCurve& curve, size_t pointIndex);
		double solveSwapRate(
			const YieldCurve& curve,
			const std::optional<double>& first_fixing,
			const std::optional<double>& second_fixing);

		bool operator == (const IrSwap& rhs) const { return endDate() == rhs.endDate(); }
		bool operator <  (const IrSwap& rhs) const { return endDate() <  rhs.endDate(); }

	private:
		IrSwapLegFixed fixedLeg_ {};
		IrSwapLegFloating floatingLeg_ {};
	};
}

#endif // __jetblack__rates__ir_swap_hpp
