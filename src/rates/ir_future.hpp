#ifndef __jetblack__rates__ir_future_hpp
#define __jetblack__rates__ir_future_hpp

#include <chrono>
#include <set>

#include "dates/terms.hpp"
#include "dates/schedules.hpp"
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

	class IrFuture 
	{
	public:

		IrFuture() = default;
		
		IrFuture(
			const year_month_day& expiryDate,
			EDayCount dayCount,
			double price,
			double convexity,
			EDateRule dateRule,
			const std::set<year_month_day>& holidays,
			const time_unit_t& spot);

		const year_month_day& startDate() const { return deposit_.startDate(); }
		const year_month_day& endDate() const { return deposit_.endDate(); }
		EDayCount dayCount() const { return deposit_.dayCount(); }
		double rate() const { return deposit_.rate(); }
		void rate(double rate) { deposit_.rate(rate); }

		double value(const YieldCurve& curve) const;
		double calculateZeroRate(const YieldCurve& curve) const;
		double solveZeroRate(const YieldCurve& curve, size_t pointIndex) const;

		bool operator == (const IrFuture& rhs) const
		{
			return deposit_ == rhs.deposit_;
		}

		bool operator < (const IrFuture& rhs) const
		{
			return deposit_ < rhs.deposit_;
		}

	private:
		Deposit deposit_ {};
	};
}

#endif // __jetblack__rates__ir_future_hpp
