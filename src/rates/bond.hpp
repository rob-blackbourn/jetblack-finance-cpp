#ifndef __jetblack__rates__bond_hpp
#define __jetblack__rates__bond_hpp

#include <chrono>
#include <set>
#include <set>
#include <vector>

#include "dates/schedules.hpp"
#include "dates/terms.hpp"

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	class YieldCurve;

	class Bond
	{
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
			double faceValue,
			EDateRule dateRule)
			:	schedule_(schedule),
				firstAccrualDate_(firstAccrualDate),
				maturityDate_(maturityDate),
				couponRate_(couponRate),
				couponFrequency_(couponFrequency),
				dayCount_(dayCount),
				stubType_(stubType),
				faceValue_(faceValue),
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
			double faceValue,
			EDateRule dateRule,
			const std::set<year_month_day>& holidays);

		double value(const year_month_day& valueDate, const YieldCurve& curve) const;
		double value(const YieldCurve& curve) const;
		double value(const year_month_day& valueDate, double yield) const;
		double yield(const year_month_day& valueDate, double price) const;

		double accruedInterest(const year_month_day& valueDate) const;

		const std::vector<year_month_day>& schedule() const { return schedule_; }
		const year_month_day& firstAccrualDate() const {return firstAccrualDate_; }
		const year_month_day& maturityDate() const { return maturityDate_; }
		double couponRate() const { return couponRate_; }
		EFrequency couponFrequency() const { return couponFrequency_; }
		EDayCount dayCount() const { return dayCount_; }
		EStubType stubType() const { return stubType_; }
		double faceValue() const { return faceValue_; }
		EDateRule dateRule() const { return dateRule_; }

	private:
		std::vector<year_month_day> schedule_ {};
		year_month_day firstAccrualDate_ {};
		year_month_day maturityDate_ {};
		double couponRate_ {0.0};
		EFrequency couponFrequency_ {EFrequency::Annual};
		EDayCount dayCount_ {EDayCount::Actual_d365};
		EStubType stubType_ {EStubType::ShortFirst};
		double faceValue_ {0.0};
		EDateRule dateRule_ {EDateRule::ModFollowing};
	};
}

#endif // __jetblack__rates__bond_hpp
