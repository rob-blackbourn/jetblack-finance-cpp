#ifndef __jetblack__rates__ir_swap_hpp
#define __jetblack__rates__ir_swap_hpp

#include <chrono>
#include <optional>

#include "rates/instrument.hpp"
#include "rates/ir_swap_leg_fixed.hpp"
#include "rates/ir_swap_leg_floating.hpp"
#include "rates/ir_swap.hpp"

namespace rates
{
	using namespace std::chrono;

	class YieldCurve;

	class IrSwap : public Instrument
	{
	private:
		IrSwapLegFixed fixedLeg_ {};
		IrSwapLegFloating floatingLeg_ {};
		
	public:
		IrSwap() = default;

		IrSwap(
			const IrSwapLegFixed& fixedLeg,
			const IrSwapLegFloating& floatingLeg);

		IrSwap(
			double notional,
			double fixedRate,
			double spread,
			const year_month_day& startDate,
			const time_unit_t& tenor,
			EFrequency frequency,
			EStubType stubType,
			EDateRule dateRule,
			EDayCount dayCount,
			const time_unit_t& fixLag,
			const std::set<year_month_day>& holidays);

		IrSwap(
			double notional,
			double fixedRate,
			double spread,
			const year_month_day& startDate,
			const year_month_day& endDate,
			EFrequency frequency,
			EStubType stubType,
			EDateRule dateRule,
			EDayCount dayCount,
			const time_unit_t& fixLag,
			const std::set<year_month_day>& holidays);

		virtual ~IrSwap() override
		{
		}

		double value(const YieldCurve& curve) const;
		double value(const year_month_day& valueDate, const YieldCurve& curve) const;

		IrSwapLegFixed& fixedLeg() { return fixedLeg_; }
		const IrSwapLegFixed& fixedLeg() const { return fixedLeg_; }

		IrSwapLegFloating& floatingLeg() { return floatingLeg_; }
		const IrSwapLegFloating& floatingLeg() const { return floatingLeg_; }

		virtual const year_month_day& endDate() const override { return fixedLeg_.endDate(); }

		virtual double rate() const override { return fixedLeg_.rate(); }
		virtual void rate(double rate) override { fixedLeg_.rate(rate); }

		double calculateZeroRate(const YieldCurve& curve) const;
		virtual double solveZeroRate(YieldCurve& curve, size_t index) override;
		double solveSwapRate(
			const YieldCurve& curve,
			const std::optional<double>& first_fixing,
			const std::optional<double>& second_fixing);

		virtual std::shared_ptr<Instrument> clone_shared() const override
		{
			return std::make_shared<IrSwap>(*this);
		}
		virtual std::unique_ptr<Instrument> clone_unique() const override
		{
			return std::make_unique<IrSwap>(*this);
		}
	};
}

#endif // __jetblack__rates__ir_swap_hpp
