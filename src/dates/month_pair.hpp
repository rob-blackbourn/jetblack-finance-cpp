#ifndef __jetblack__dates__month_pair_hpp
#define __jetblack__dates__month_pair_hpp

#include <chrono>


namespace dates
{
	struct MonthPair
	{
		std::chrono::months short_months;
		std::chrono::months long_months;
	};
}

/*
inline
dates::MonthPair& operator>>(const std::string& lhs, dates::MonthPair& rhs)
{
	const char* str = lhs.c_str();
	char* tenorCode1, *tenorCode2;
	dates::Tenor tenor1, tenor2;

	// 1M/3M
	int matches = sscanf(str, "%s/%s", tenorCode1, *tenorCode2);
	if (matches == 1)
	{
		tenor1.spot = true;
		tenor1.days = tenor1.weeks = tenor1.months = 0;
		tenorCode1 >> tenor2;
	}
	else if (matches == 2)
	{
		tenorCode1 >> tenor1;
		tenorCode2 >> tenor2;
	}
	else
		throw "invalid month pair";

	if (tenor1.days != 0 || tenor1.weeks != 0 || tenor2.days != 0 && tenor2.weeks != 0)
		throw "only month tenors allowed";

	if (tenor1.months > tenor2.months)
		throw "tenors in wrong order";

	rhs.short_months = tenor1.months;
	rhs.long_months = tenor2.months;

	return rhs;
}
*/

#endif // __jetblack__dates__month_pair_hpp
