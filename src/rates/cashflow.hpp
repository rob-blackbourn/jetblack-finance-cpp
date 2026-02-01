#ifndef __jetblack__rates__cashflow_hpp
#define __jetblack__rates__cashflow_hpp

#include <chrono>
#include <set>
#include <vector>

#include "dates/terms.hpp"
#include "dates/schedules.hpp"

namespace rates
{
	using namespace std::chrono;
	using namespace dates;

	class YieldCurve;

	/*
	 * Class to represent an interest-like cashflow. Could be fixed or floating. Probably should be extended to cope
	 * with principal payments at some point.
	 * It is assumed that the payment is made on the last day of the cashflow.
	 */

	class CashFlow
	{
	private:
		year_month_day firstAccrualDate_ {};
		year_month_day endDate_ {};
		double notional_ {0};
		EDayCount dayCount_ {EDayCount::Actual_d365};
		double rate_ {0};
		double flow_ {0};

	public:
		CashFlow() = default;

		CashFlow(
			const year_month_day& firstAccrualDate,
			const year_month_day& endDate,
			double notional,
			EDayCount dayCount,
			double rate,
			double flow)
			:	firstAccrualDate_(firstAccrualDate),
				endDate_(endDate),
				notional_(notional),
				dayCount_(dayCount),
				rate_(rate) ,
				flow_(flow)
		{
		}

		CashFlow(
			const year_month_day& firstAccrualDate,
			const year_month_day& endDate,
			double notional,
			EDayCount dayCount,
			double rate)
			:	CashFlow(
					firstAccrualDate,
					endDate,
					notional,
					dayCount,
					rate,
					calculateFlow(rate, notional, firstAccrualDate, endDate, dayCount)
				) 
		{
		}

		CashFlow(
			const year_month_day& firstAccrualDate,
			const year_month_day& endDate,
			double notional,
			EDayCount dayCount) 
			:	firstAccrualDate_(firstAccrualDate),
				endDate_(endDate),
				notional_(notional),
				dayCount_(dayCount)
		{
		}

		CashFlow(
			const year_month_day& firstAccrualDate,
			const year_month_day& endDate,
			EDayCount dayCount,
			double flow)
			:	firstAccrualDate_(firstAccrualDate),
				endDate_(endDate),
				dayCount_(dayCount),
				flow_(flow)
		{
		}

		CashFlow(
			const year_month_day& firstAccrualDate,
			const year_month_day& endDate,
			double flow)
			:	firstAccrualDate_(firstAccrualDate),
				endDate_(endDate),
				flow_(flow)
		{
		}

		double rate() const { return rate_; }
		void rate(double rate) { rate_ = rate; flow_ = calculateFlow(); }

		double notional() const { return notional_; }

		EDayCount dayCount() const { return dayCount_; }
		void dayCount(EDayCount dayCount) { dayCount_ = dayCount; }

		const year_month_day& firstAccrualDate() const { return firstAccrualDate_; }

		const year_month_day& endDate() const { return endDate_; }

		double flow() const { return flow_; }


		double value(const YieldCurve& curve) const;
		double value(const year_month_day& valueDate, const YieldCurve& curve) const;
		double value(const year_month_day& valueDate, double yield, EFrequency frequency) const;

	private:
		double calculateFlow()
		{
			return calculateFlow(rate_, notional_,firstAccrualDate_, endDate_, dayCount_);
		}

		static double calculateFlow(double rate, double notional, const year_month_day& firstAccrualDate, const year_month_day& endDate, EDayCount dayCount)
		{
			return rate * notional * yearFrac(firstAccrualDate, endDate, dayCount);
		}
	};

	double valueCashflows(const std::vector<CashFlow>& cashflows, const YieldCurve& curve);
	double calculateAccrued(const year_month_day& valueDate, const std::vector<CashFlow>& cashflows);
}

#endif // __jetblack__rates__cashflow_hpp
