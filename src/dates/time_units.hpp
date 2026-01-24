#ifndef __jetblack__dates__frequency_hpp
#define __jetblack__dates__frequency_hpp

#include <chrono>
#include <stdexcept>
#include <variant>

#include "dates/arithmetic.hpp"
#include "dates/business_days.hpp"
#include "dates/adjustments.hpp"

#include "stdext/match.hpp"

namespace dates
{
	using namespace std::chrono;
	using namespace stdext;
	
	using time_unit_t = std::variant<days, weeks, months, years>;

	inline time_unit_t operator - (const time_unit_t& offset)
	{
		return std::visit( match {
			[](const days& i) { return time_unit_t{-i}; },
			[](const weeks& i) { return time_unit_t{-i}; },
			[](const months& i) { return time_unit_t{-i}; },
			[](const years& i) { return time_unit_t{-i}; },
		}, offset);
	}

	inline time_unit_t operator * (const time_unit_t& lhs, int rhs)
	{
		return std::visit( match {
			[rhs](const days& i) { return time_unit_t{i * rhs}; },
			[rhs](const weeks& i) { return time_unit_t{i * rhs}; },
			[rhs](const months& i) { return time_unit_t{i * rhs}; },
			[rhs](const years& i) { return time_unit_t{i * rhs}; },
		}, lhs);
	}	

	inline time_unit_t operator / (const time_unit_t& lhs, int rhs)
	{
		return std::visit( match {
			[rhs](const days& i) { return time_unit_t{i / rhs}; },
			[rhs](const weeks& i) { return time_unit_t{i / rhs}; },
			[rhs](const months& i) { return time_unit_t{i / rhs}; },
			[rhs](const years& i) { return time_unit_t{i / rhs}; },
		}, lhs);
	}	

	inline
	year_month_day add(
		const year_month_day& date,
		const time_unit_t& offset,
		bool eom,
		EDateRule dateRule,
		const std::set<year_month_day>& holidays = {})
	{
		return std::visit( match {
			[&date, &holidays](days i) { return addBusinessDays(date, i, holidays); },
			[&date, dateRule, &holidays](weeks i) { return addWeeks(date, i, dateRule, holidays); },
			[&date, eom, dateRule, &holidays](months i) { return addMonths(date, i, eom, dateRule, holidays); },
			[&date, eom, dateRule, &holidays](years i) { return addYears(date, i, eom, dateRule, holidays); },
		}, offset);
	}
}

inline dates::time_unit_t& operator >> (const std::string& lhs, dates::time_unit_t& rhs)
{
	size_t len = lhs.size();

	if (len < 2) throw "invalid string for TimePeriod";

	std::string count = lhs.substr(0, len - 1);
	auto n = atoi(count.c_str());

	switch (lhs.at(len - 1))
	{
	case 'd':
	case 'D':
		rhs = std::chrono::days{n};
	case 'w':
	case 'W':
		rhs = std::chrono::weeks{n};
	case 'm':
	case 'M':
		rhs = std::chrono::months{n};
	case 'y':
	case 'Y':
		rhs = std::chrono::years{n};
	default:
		throw std::runtime_error("invalid unit for TimePeriod");
	}

	return rhs;
}

inline std::string& operator << (std::string& lhs, const dates::time_unit_t& rhs)
{
	using namespace std::chrono;

	std::stringstream ss;

	std::visit( stdext::match {
		[&ss](const days& i) { ss << i; },
		[&ss](const weeks& i) { ss << i; },
		[&ss](const months& i) { ss << i; },
		[&ss](const years& i) { ss << i; },
	}, rhs);

	lhs = ss.str();
	return lhs;
}

inline std::ostream& operator<<(std::ostream& lhs, const dates::time_unit_t& rhs)
{
	std::string s;
	s << rhs;
	lhs << s;
	return lhs;
}

#endif // __jetblack__dates__frequency_hpp
