#ifndef __jetblack__dates__adjustments_hpp
#define __jetblack__dates__adjustments_hpp

#include <cstring>

#include <algorithm>
#include <chrono>
#include <set>
#include <string>
#include <type_traits>
#include <vector>

#include "dates/arithmetic.hpp"
#include "dates/business_days.hpp"

namespace dates
{
    using namespace std::chrono;

	enum class EDateRule
	{ 
		None = 0,
		Preceding = 1,
		Following = 2,
		ModPreceding = 3,
		ModFollowing = 4
	};

	inline
	year_month_day
	adjust(
		const year_month_day& d,
		EDateRule dateRule,
		const std::set<year_month_day>& holidays)
	{
		if(isBusinessDay(d, holidays) || dateRule == EDateRule::None)
			return d;

		switch (dateRule)
		{
		case EDateRule::Following:
			return addBusinessDays(d, days{1}, holidays);
		case EDateRule::Preceding:
			return addBusinessDays(d, days{-1}, holidays);
		case EDateRule::ModFollowing:
			{
				auto adj = addBusinessDays(d, days{1}, holidays);

				if(adj.month() == d.month())
					return adj;
				else
					return addBusinessDays(d, days{-1}, holidays);
			}
		case EDateRule::ModPreceding:
			{
				auto adj = addBusinessDays(d, days{-1}, holidays);	
				
				if(adj.month() == d.month())
					return adj;
				else
					return addBusinessDays(d, days{1}, holidays);
			}
		default:
			throw std::invalid_argument("unknown business date adjustment");
		}
	}

	inline
	year_month_day
	addMonths(
		const year_month_day& date,
		const months& m,
		bool eom,
		EDateRule dateRule,
		const std::set<year_month_day>& holidays = {})
	{
		return adjust(addMonths(date, m, eom), dateRule, holidays);
	}

	inline
	year_month_day
	addYears(
		const year_month_day& date,
		const years& y,
		bool eom,
		EDateRule dateRule,
		const std::set<year_month_day>& holidays = {})
	{
		return adjust(addYears(date, y, eom), dateRule, holidays);
	}

	inline
	year_month_day
	addWeeks(
		const year_month_day& date,
		const weeks& w,
		EDateRule dateRule,
		const std::set<year_month_day>& holidays = {})
	{
		return adjust(addWeeks(date, w), dateRule, holidays);
	}
}

static const struct { const char* string_type; dates::EDateRule enum_type; } DateRule_TypeMap[] =
{
	{"NONE",			dates::EDateRule::None},
	{"P",				dates::EDateRule::Preceding},
	{"PRECEDING",		dates::EDateRule::Preceding},
	{"F",				dates::EDateRule::Following},
	{"FOLLOWING",		dates::EDateRule::Following},
	{"MF",				dates::EDateRule::ModFollowing},
	{"MOD FOLLOWING",	dates::EDateRule::ModFollowing},
	{"MP",				dates::EDateRule::ModPreceding},
	{"MOD PRECEDING",	dates::EDateRule::ModPreceding}
};

inline std::string& operator<<(std::string& lhs, const dates::EDateRule& rhs)
{
	for (size_t i = 0; i < sizeof(DateRule_TypeMap) / sizeof(DateRule_TypeMap[0]); ++i)
		if (rhs == DateRule_TypeMap[i].enum_type)
			return lhs = DateRule_TypeMap[i].string_type;

	throw std::invalid_argument("unknown enum");
}

inline std::ostream& operator<<(std::ostream& lhs, const dates::EDateRule& rhs)
{
	std::string s;
	s << rhs;
	lhs << s;
	return lhs;
}

#endif // __jetblack__dates__adjustments_hpp
