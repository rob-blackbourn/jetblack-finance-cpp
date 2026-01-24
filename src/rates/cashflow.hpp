#ifndef __jetblack__rates__cashflow_hpp
#define __jetblack__rates__cashflow_hpp

#include <chrono>
#include <set>
#include <vector>

#include "dates/frequency.hpp"
#include "dates/terms.hpp"

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
	public:
		CashFlow() = default;

		CashFlow(
			const year_month_day& startDate,
			const year_month_day& endDate,
			double notional,
			EDayCount dayCount,
			double rate,
			double flow)
			:	startDate_(startDate),
				endDate_(endDate),
				notional_(notional),
				dayCount_(dayCount),
				rate_(rate) ,
				flow_(flow)
		{
		}

		CashFlow(
			const year_month_day& startDate,
			const year_month_day& endDate,
			double notional,
			EDayCount dayCount,
			double rate)
			:	CashFlow(
					startDate,
					endDate,
					notional,
					dayCount,
					rate,
					calculateFlow(rate, notional, startDate, endDate, dayCount)
				) 
		{
		}

		CashFlow(
			const year_month_day& startDate,
			const year_month_day& endDate,
			double notional,
			EDayCount dayCount) 
			:	startDate_(startDate),
				endDate_(endDate),
				notional_(notional),
				dayCount_(dayCount)
		{
		}

		CashFlow(
			const year_month_day& startDate,
			const year_month_day& endDate,
			EDayCount dayCount,
			double flow)
			:	startDate_(startDate),
				endDate_(endDate),
				dayCount_(dayCount),
				flow_(flow)
		{
		}

		CashFlow(
			const year_month_day& startDate,
			const year_month_day& endDate,
			double flow)
			:	startDate_(startDate),
				endDate_(endDate),
				flow_(flow)
		{
		}

		double rate() const { return rate_; }
		void rate(double rate) { rate_ = rate; flow_ = calculateFlow(); }

		double notional() const { return notional_; }

		EDayCount dayCount() const { return dayCount_; }
		void dayCount(EDayCount dayCount) { dayCount_ = dayCount; }

		const year_month_day& startDate() const { return startDate_; }

		const year_month_day& endDate() const { return endDate_; }

		double flow() const { return flow_; }


		double value(const YieldCurve& curve) const;
		double value(const year_month_day& valueDate, const YieldCurve& curve) const;
		double value(const year_month_day& valueDate, double yield, EFrequency frequency) const;

	private:
		double calculateFlow()
		{
			return calculateFlow(rate_, notional_,startDate_, endDate_, dayCount_);
		}

		static double calculateFlow(double rate, double notional, const year_month_day& startDate, const year_month_day& endDate, EDayCount dayCount)
		{
			return rate * notional * yearFrac(startDate, endDate, dayCount);
		}

	private:
		year_month_day startDate_ {};
		year_month_day endDate_ {};
		double notional_ {0};
		EDayCount dayCount_ {EDayCount::Actual_d365};
		double rate_ {0};
		double flow_ {0};
	};

	double valueCashflows(const std::vector<CashFlow>& cashflows, const YieldCurve& curve);
	double calculateAccrued(const year_month_day& valueDate, const std::vector<CashFlow>& cashflows);
}

#endif // __jetblack__rates__cashflow_hpp
