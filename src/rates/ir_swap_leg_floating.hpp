#ifndef __jetblack__rates__ir_swap_leg_floating_hpp
#define __jetblack__rates__ir_swap_leg_floating_hpp

#include <chrono>
#include <optional>
#include <set>
#include <vector>

#include "dates/schedules.hpp"
#include "dates/terms.hpp"

#include "rates/ir_swap_leg.hpp"
#include "rates/dated_rate.hpp"

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	class YieldCurve;

	class IrSwapLegFloating : public IrSwapLeg
	{
	public:
		IrSwapLegFloating() = default;

		IrSwapLegFloating(
			const year_month_day& startDate,
			const time_unit_t& tenor,
			EFrequency frequency,
			EStubType stubType,
			EDateRule dateRule,
			const std::set<year_month_day>& holidays,
			EDayCount dayCount,
			double notional,
			const time_unit_t& fixLag,
			double spread);

		IrSwapLegFloating(
			const year_month_day& startDate,
			const year_month_day& endDate,
			EFrequency frequency,
			EStubType stubType,
			EDateRule dateRule,
			const std::set<year_month_day>& holidays,
			EDayCount dayCount,
			double notional,
			const time_unit_t& fixLag,
			double spread);

		virtual double value(const year_month_day& valueDate, const YieldCurve& curve) const;
		virtual double value(const YieldCurve& curve) const;
		virtual double accrued(const year_month_day& valueDate) const;

		void reset(const YieldCurve& curve, const std::optional<double>& first_fixing, const std::optional<double>& second_fixing);
		std::pair<std::optional<double>,std::optional<double>> getCurrentFixings(const year_month_day& valueDate) const;

		const std::vector<DatedRate>& fixings() const { return fixings_; }
		double spread() const { return spread_; }
		const time_unit_t& fixLag() const { return  fixLag_; }

	private:
		double spread_ {0};
		time_unit_t fixLag_ {days{0}};
		std::vector<DatedRate> fixings_ {};
	};
}

#endif // __jetblack__rates__ir_swap_leg_floating_hpp
