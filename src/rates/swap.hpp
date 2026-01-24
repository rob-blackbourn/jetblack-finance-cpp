#ifndef __jetblack__rates__swap_hpp
#define __jetblack__rates__swap_hpp

#include <chrono>

#include "rates/swap_leg_fixed.hpp"
#include "rates/swap_leg_floating.hpp"

namespace rates
{
	class YieldCurve;

	class Swap
	{
	public:
		Swap() = default;

		Swap(
			const SwapLegFixed& fixedLeg,
			const SwapLegFloating& floatingLeg)
			:	fixedLeg_(fixedLeg),
				floatingLeg_(floatingLeg)
		{
		}

		double value(const YieldCurve& curve) const;

		SwapLegFixed& fixedLeg() { return fixedLeg_; }
		const SwapLegFixed& fixedLeg() const { return fixedLeg_; }

		SwapLegFloating& floatingLeg() { return floatingLeg_; }
		const SwapLegFloating& floatingLeg() const { return floatingLeg_; }

		const std::chrono::year_month_day& endDate() const { return fixedLeg_.endDate(); }

		double calculateZeroRate(const YieldCurve& curve) const;
		double solveZeroRate(YieldCurve& curve, size_t pointIndex) const;
		double solveSwapRate(YieldCurve& curve, const swap_fixing_t& swap_fixing);

		bool operator == (const Swap& rhs) const { return endDate() == rhs.endDate(); }
		bool operator <  (const Swap& rhs) const { return endDate() <  rhs.endDate(); }

	private:
		SwapLegFixed fixedLeg_ {};
		SwapLegFloating floatingLeg_ {};
	};

	bool compareSwaps(const Swap& s1, const Swap& s2);
}

#endif // __jetblack__rates__swap_hpp
