#pragma once

#include "targetver.h"

#define NOMINMAX
#define _USE_MATH_DEFINES

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// TODO: reference additional headers your program requires here
#include <sys/types.h>
#include<sys/timeb.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <atlbase.h>
#include <atlstr.h>

#include <atlcomtime.h>
#include <atlcomcli.h>

#include <iostream>
#include <locale>
#include <string>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <functional>
#include <deque>
#include <stdexcept>
#include <limits>
#include <cmath>
#include <cfloat>
#include <cstring>

#include <dates/dates.h>
#include <dates/tenor.h>

#include <rates/rates.h>
#include <rates/exp_interp.h>
#include <rates/lin_interp.h>
#include <rates/df_curve.h>
#include <rates/deposit.h>
#include <rates/par_swap.h>
#include <rates/yield_curve.h>

#include "ir_swap.h"

template <typename T>
class add_constant
{
private:
	T value;
public:
	add_constant(const T& value)
		: value(value)
	{
	}

	void operator() (T& elem) const
	{
		elem += value;
	}
};

namespace swaps
{
	double ir_swap_pv(
		const COleDateTime& effective_date,
		unsigned int spot_days,
		const rates::df_curve_t& df_curve,
		dates::EDayCount df_daycount,
		double notional,
		dates::EFrequency fixed_frequency,
		double fixed_rate,
		dates::EFrequency floating_frequency,
		double spread,
		double first_fixing,
		unsigned int months_duration,
		dates::EDayCount daycount,
		dates::EDateRule dateRule,
		const std::vector<COleDateTime>& holidays)
	{
		// Calculate the spot date, the term, and the discount factor.
		COleDateTime spot_date = dates::addBusinessDays(effective_date, spot_days, holidays);
		double spot_t = dates::yearsBetween(effective_date, spot_date, df_daycount);
		double spot_df = df_curve.interp_df(spot_t);

		// Calculate the maturityDate date
		COleDateTime maturity_date = dates::addMonths(spot_date, static_cast<int>(months_duration));

		// -- Fixed side --
		// Generate the schedule from spot date to maturityDate date
		std::vector<COleDateTime> fixed_sched = dates::generateSchedule(spot_date, maturity_date, false, fixed_frequency, true);
		fixed_sched = std::vector<COleDateTime>(fixed_sched.begin() + 1, fixed_sched.end());
		// Adjust the schedule with the given holidays and date rule
		fixed_sched = dates::adjustSchedule(fixed_sched, dateRule, holidays);
		// Calculate the schedule terms between spot and the schedule date
		std::vector<double> fixed_terms = dates::yearsBetween(effective_date, fixed_sched, daycount);

		// Calculate the cash flows
		std::vector<double> fixed_cash_flows(fixed_terms.size());
		for (size_t i = 0; i < fixed_terms.size(); ++i)
		{
			double cash_flow = notional / (int) fixed_frequency;
			cash_flow *= fixed_rate;
			if (i == fixed_terms.size() - 1)
				cash_flow += notional;
			fixed_cash_flows[i] = cash_flow;
		}

		// Generate the pv to the effective date
		double fixed_pv = rates::pv_cash_flows(fixed_terms, fixed_cash_flows, df_curve);
		fixed_pv /= spot_df;

		// -- Floating side --
		// Generate the schedule from spot date to maturityDate date
		std::vector<COleDateTime> floating_sched = dates::generateSchedule(spot_date, maturity_date, false, floating_frequency, true);
		floating_sched = std::vector<COleDateTime>(floating_sched.begin() + 1, floating_sched.end());
		// Adjust the schedule with the given holidays and date rule
		floating_sched = dates::adjustSchedule(floating_sched, dateRule, holidays);
		// Calculate the schedule terms between spot and the schedule date
		std::vector<double> floating_terms = dates::yearsBetween(effective_date, floating_sched, daycount);

		// Calculate the fixings
		std::vector<double> floating_fixings(floating_terms.size());
		double prev_t = spot_t, prev_df = spot_df;
		for (size_t i = 0; i < floating_terms.size(); ++i)
		{
			double t = floating_terms[i];
			double df = df_curve.interp_df(t);

			double rate;
			if (i == 0)
				rate = first_fixing;
			else
				rate = rates::get_rate_from_df(df/prev_df, t-prev_t, rates::ECompounding_Simple, dates::EFrequency_Annual);

			floating_fixings[i] = spread + rate;

			prev_t = t;
			prev_df = df;
		}

		// Calculate the cash flows
		std::vector<double> floating_cash_flows(floating_terms.size());
		for (size_t i = 0; i < floating_terms.size(); ++i)
		{
			double fixing = floating_fixings[i];
			double cash_flow = notional / (int) floating_frequency;
			cash_flow *= fixing;
			if (i == floating_terms.size() - 1)
				cash_flow += notional;

			floating_cash_flows[i] = cash_flow;
		}

		// Generate the pv to the effective date
		double floating_pv = rates::pv_cash_flows(floating_terms, floating_cash_flows, df_curve);
		floating_pv /= spot_df;

		double pv = fixed_pv - floating_pv;
		pv *= spot_df;

		return pv;
	}
}