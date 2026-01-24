#pragma once

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
		dates::EDateRule date_rule,
		const std::vector<COleDateTime>& holidays);
}