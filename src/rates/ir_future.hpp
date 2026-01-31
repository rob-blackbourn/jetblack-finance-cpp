#ifndef __jetblack__rates__ir_future_hpp
#define __jetblack__rates__ir_future_hpp

#include <chrono>
#include <set>

#include "dates/terms.hpp"
#include "dates/schedules.hpp"

#include "rates/instrument.hpp"
#include "rates/deposit.hpp"

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	class YieldCurve;

	/*
	 * An interest rate future is just a deposit, which begins spot days after the expiry of the future and ends three 
	 * months later. The underlying deposit assumes that $1 is paid on the start date and $1 plus interest is paid back
	 * on the end date.
	 */

	class IrFuture : public Instrument
	{
	public:

		IrFuture() = default;
		
		IrFuture(
			double notional,
			double price,
			const year_month_day& expiryDate,
			const months& nMonths,
			EDayCount dayCount,
			EDateRule dateRule,
			const time_unit_t& spotLead,
			const std::set<year_month_day>& holidays);

		IrFuture(
			double notional,
			double price,
			const year_month& expiry,
			EDayCount dayCount,
			EDateRule dateRule,
			const time_unit_t& spotLead,
			const std::set<year_month_day>& holidays);

		IrFuture(const IrFuture& irFuture) = default;

		virtual ~IrFuture() override
		{
		}

		double notional() const { return deposit_.notional(); }
		const year_month_day& startDate() const { return deposit_.startDate(); }
		virtual const year_month_day& endDate() const override { return deposit_.endDate(); }
		EDayCount dayCount() const { return deposit_.dayCount(); }
		
		virtual double rate() const override { return deposit_.rate(); }
		virtual void rate(double rate) override { deposit_.rate(rate); }

		double value(const YieldCurve& curve) const;
		double calculateZeroRate(const YieldCurve& curve) const;
		virtual double solveZeroRate(YieldCurve& curve, size_t index) override;

		virtual std::shared_ptr<Instrument> clone_shared() const override
		{
			return std::make_shared<IrFuture>(*this);
		}
		virtual std::unique_ptr<Instrument> clone_unique() const override
		{
			return std::make_unique<IrFuture>(*this);
		}

	private:
		Deposit deposit_ {};
	};
}

#endif // __jetblack__rates__ir_future_hpp
