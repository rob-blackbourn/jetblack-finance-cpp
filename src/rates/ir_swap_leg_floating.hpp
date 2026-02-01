#ifndef __jetblack__rates__ir_swap_leg_floating_hpp
#define __jetblack__rates__ir_swap_leg_floating_hpp

#include <chrono>
#include <optional>
#include <set>
#include <vector>

#include "dates/schedules.hpp"
#include "dates/terms.hpp"

#include "rates/ir_swap_leg.hpp"

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	class YieldCurve;

	class IrSwapLegFloating : public IrSwapLeg
	{
	private:
		double spread_ {0};
		time_unit_t fixLag_ {days{0}};
		std::vector<year_month_day> fixingSchedule_ {};
		
	public:
		IrSwapLegFloating() = default;

		IrSwapLegFloating(
			double notional,
			double spread,
			const year_month_day& firstAccrualDate,
			const time_unit_t& tenor,
			EFrequency frequency,
			EStubType stubType,
			EDateRule dateRule,
			EDayCount dayCount,
			const time_unit_t& fixLag,
			const std::set<year_month_day>& holidays);

		IrSwapLegFloating(
			double notional,
			double spread,
			const year_month_day& firstAccrualDate,
			const year_month_day& maturityDate,
			EFrequency frequency,
			EStubType stubType,
			EDateRule dateRule,
			EDayCount dayCount,
			const time_unit_t& fixLag,
			const std::set<year_month_day>& holidays);

		virtual double value(const year_month_day& valueDate, const YieldCurve& curve) const;
		virtual double value(const YieldCurve& curve) const;
		virtual double accrued(const YieldCurve& curve, const year_month_day& valueDate) const;

		std::pair<std::optional<double>,std::optional<double>> getCurrentFixings(const YieldCurve& curve, const year_month_day& valueDate) const;

		const std::vector<year_month_day>& fixingSchedule() const { return fixingSchedule_; }
		double spread() const { return spread_; }
		const time_unit_t& fixLag() const { return  fixLag_; }

	private:
		std::vector<double> getFixingRates(const YieldCurve& curve) const;
	};
}

#endif // __jetblack__rates__ir_swap_leg_floating_hpp
