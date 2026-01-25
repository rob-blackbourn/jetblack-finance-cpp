#ifndef __jetblack__rates__deposit_hpp
#define __jetblack__rates__deposit_hpp

#include <chrono>
#include <set>

#include "dates/terms.hpp"
#include "dates/schedules.hpp"

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	class YieldCurve;

	/*
	 * Deposit rate is a payment of $1 on the start date and receipt of $1 plus interest on the end date.
	 */

	class Deposit
	{
	public:
		Deposit() = default;

		Deposit(
			const year_month_day& startDate,
			const year_month_day& endDate,
			EDayCount dayCount,
			double rate);

		Deposit(
			const year_month_day& startDate,
			const time_unit_t& tenor,
			EDayCount dayCount,
			double rate,
			EDateRule dateRule,
			const std::set<year_month_day>& holidays);

		Deposit(const Deposit& deposit) = default;

		const year_month_day& startDate() const { return startDate_; }
		const year_month_day& endDate() const { return endDate_; }
		EDayCount dayCount() const { return dayCount_; }
		double rate() const { return rate_; }
		void rate(double rate) { rate_ = rate; }

		double value(const YieldCurve& curve) const;
		double calculateZeroRate(const YieldCurve& curve) const;
		double solveZeroRate(const YieldCurve& curve, size_t pointIndex) const;

		bool operator == (const Deposit& rhs) const
		{
			return endDate_ == rhs.endDate_;
		}

		bool operator < (const Deposit& rhs) const
		{
			return endDate_ < rhs.endDate_;
		}

	private:
		year_month_day startDate_ {};
		year_month_day endDate_ {};
		EDayCount dayCount_ {EDayCount::Actual_d365};
		double rate_ {0};
	};
}

#endif // __jetblack__rates__deposit_hpp
