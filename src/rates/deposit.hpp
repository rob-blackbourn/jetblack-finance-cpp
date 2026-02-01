#ifndef __jetblack__rates__deposit_hpp
#define __jetblack__rates__deposit_hpp

#include <chrono>
#include <set>

#include "dates/terms.hpp"
#include "dates/schedules.hpp"

#include "rates/instrument.hpp"

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	class YieldCurve;

	/*
	 * Deposit rate is a payment of $1 on the start date and receipt of $1 plus interest on the end date.
	 */

	class Deposit : public Instrument
	{
	private:
		double notional_ {1};
		double rate_ {0};
		year_month_day startDate_ {};
		year_month_day endDate_ {};
		EDayCount dayCount_ {EDayCount::Actual_d365};
		
	public:
		Deposit() = default;

		Deposit(
			double notional,
			double rate,
			const year_month_day& startDate,
			const year_month_day& endDate,
			EDayCount dayCount);

		Deposit(
			double notional,
			double rate,
			const year_month_day& startDate,
			const time_unit_t& tenor,
			EDayCount dayCount,
			EDateRule dateRule,
			const std::set<year_month_day>& holidays);

		Deposit(const Deposit& deposit) = default;

		virtual ~Deposit() override
		{
		}

		double notional() const { return notional_; }
		const year_month_day& startDate() const { return startDate_; }
		virtual const year_month_day& endDate() const override { return endDate_; }
		EDayCount dayCount() const { return dayCount_; }
		virtual double rate() const override { return rate_; }
		virtual void rate(double rate) override { rate_ = rate; }

		virtual double value(const YieldCurve& curve) const override;
		double calculateZeroRate(const YieldCurve& curve) const;

		virtual std::shared_ptr<Instrument> clone_shared() const override
		{
			return std::make_shared<Deposit>(*this);
		}
		virtual std::unique_ptr<Instrument> clone_unique() const override
		{
			return std::make_unique<Deposit>(*this);
		}
	};
}

#endif // __jetblack__rates__deposit_hpp
