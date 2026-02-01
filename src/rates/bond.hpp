#ifndef __jetblack__rates__bond_hpp
#define __jetblack__rates__bond_hpp

#include <chrono>
#include <memory>
#include <set>
#include <vector>

#include "dates/schedules.hpp"
#include "dates/terms.hpp"

#include "rates/instrument.hpp"

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	class YieldCurve;

	class Bond : public Instrument
	{
	private:
		std::vector<year_month_day> schedule_ {};
		year_month_day firstAccrualDate_ {};
		year_month_day maturity_ {};
		double couponRate_ {0.0};
		EFrequency couponFrequency_ {EFrequency::Annual};
		EDayCount dayCount_ {EDayCount::Actual_d365};
		EStubType stubType_ {EStubType::ShortFirst};
		double notional_ {1.0};
		EDateRule dateRule_ {EDateRule::ModFollowing};

	public:
		Bond() = default;
		
		Bond(
			const std::vector<year_month_day>& schedule,
			const year_month_day& firstAccrualDate,
			const year_month_day& maturityDate,
			double couponRate,
			EFrequency couponFrequency,
			EDayCount dayCount,
			EStubType stubType,
			double notional,
			EDateRule dateRule)
			:	schedule_(schedule),
				firstAccrualDate_(firstAccrualDate),
				maturity_(maturityDate),
				couponRate_(couponRate),
				couponFrequency_(couponFrequency),
				dayCount_(dayCount),
				stubType_(stubType),
				notional_(notional),
				dateRule_(dateRule)
		{
		}

		Bond(
			const year_month_day& firstAccrualDate,
			const year_month_day& maturityDate,
			double couponRate,
			EFrequency couponFrequency,
			EDayCount dayCount,
			EStubType stubType,
			double notional,
			EDateRule dateRule,
			const std::set<year_month_day>& holidays);

		virtual ~Bond() override
		{
		}

		double value(const year_month_day& valueDate, const YieldCurve& curve) const;
		virtual double value(const YieldCurve& curve) const override;
		double value(const year_month_day& valueDate, double yield) const;
		double yield(const year_month_day& valueDate, double price) const;

		double accrued(const year_month_day& valueDate) const;

		const std::vector<year_month_day>& schedule() const { return schedule_; }
		const year_month_day& firstAccrualDate() const {return firstAccrualDate_; }
		virtual const year_month_day& maturity() const override { return maturity_; }
		double couponRate() const { return couponRate_; }
		EFrequency couponFrequency() const { return couponFrequency_; }
		EDayCount dayCount() const { return dayCount_; }
		EStubType stubType() const { return stubType_; }
		double notional() const { return notional_; }
		EDateRule dateRule() const { return dateRule_; }

		virtual double rate() const override { return couponRate_; };
		virtual void rate(double rate) override { couponRate_ = rate; };

		virtual std::shared_ptr<Instrument> clone_shared() const override
		{
			return std::make_shared<Bond>(*this);
		}
		virtual std::unique_ptr<Instrument> clone_unique() const override
		{
			return std::make_unique<Bond>(*this);
		}
	};
}

#endif // __jetblack__rates__bond_hpp
