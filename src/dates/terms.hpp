#ifndef __jetblack__dates__terms_hpp
#define __jetblack__dates__terms_hpp

#include <cstring>

#include <algorithm>
#include <chrono>
#include <set>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#include "dates/arithmetic.hpp"

namespace dates
{
    using namespace std::chrono;

	enum class EDayCount
	{ 
		Actual_d360,
		Actual_d365,
		Actual_d366,
		Actual_d365_25,
		Actual_Actual,
		NY_d365,
		d30A_360, // Also know as 30/360 Bond Basis.
		d30E_d360, // Also known as 30/360 ICMA, 30/360 ISMA, 30S/360, Eurobond basis (ISDA 2006), Special German.
		d30E_d360_ISDA, // Also known as Eurobond basis (ISDA 2000), German.
		d30_d365,
		Actual_Actual_ISDA,
		Actual_Actual_ISMA,
		Actual_Actual_AFB
	};

	namespace {
		std::tuple<int, int, int> decompose(const year_month_day& date)
		{
			auto d = static_cast<unsigned int>(date.day());
			auto m = static_cast<unsigned int>(date.month());
			auto y = static_cast<int>(date.year());
			return {d, m, y};
		}
	}

	inline
	std::tuple<days,double>
	getTerm(const year_month_day& date1, const year_month_day& date2, EDayCount dayCount)
	{
		// double days, term;

		switch(dayCount)
		{
			case EDayCount::Actual_d360:
				{
					auto days_in_period = (sys_days{date2} - sys_days{date1});
                    auto days_in_year = 360.0;
					auto term = days_in_period.count() / days_in_year;
                    return { days_in_period, term };
				}
			case EDayCount::Actual_d365:
				{
					auto days_in_period = (sys_days{date2} - sys_days{date1});
                    auto days_in_year = 365.0;
					auto term = days_in_period.count() / days_in_year;
                    return { days_in_period, term };
				}
			case EDayCount::Actual_d366:
				{
					auto days_in_period = (sys_days{date2} - sys_days{date1});
                    auto days_in_year = 366.0;
					auto term = days_in_period.count() / days_in_year;
                    return { days_in_period, term };
				}
			case EDayCount::Actual_d365_25:
				{
					auto days_in_period = (sys_days{date2} - sys_days{date1});
                    auto days_in_year = 365.25;
					auto term = days_in_period.count() / days_in_year;
                    return { days_in_period, term };
				}

			case EDayCount::d30A_360:
			{
				auto [d1, m1, y1] = decompose(date1);
				auto [d2, m2, y2] = decompose(date2);

				d1 = std::min(d1, 30);

				if (d1 > 29)
				{
					d2 = std::min(d2, 30);
				}

				auto day_count = days{
					360 * (y2 - y1) + 30 * (m2 - m1) + (d2 - d1)
				};

				auto term = day_count.count() / 360.0;
				return { day_count, term };
			}

			case EDayCount::d30E_d360:
			{
				auto [d1, m1, y1] = decompose(date1);
				auto [d2, m2, y2] = decompose(date2);

				if (d1 == 31)
				{
					d1 = 30;
				}

				if (d2 == 31)
				{
					d2 = 30;
				}

				auto day_count = days{
					360 * (y2 - y1) + 30 * (m2 - m1) + (d2 - d1)
				};

				auto term = day_count.count() / 360.0;
				return { day_count, term };
			}

			case EDayCount::d30E_d360_ISDA:
			{
				auto [d1, m1, y1] = decompose(date1);
				auto [d2, m2, y2] = decompose(date2);

				if (isEndOfMonth(date1))
				{
					d1 = 30;
				}

				if (date2.month() == February && isEndOfMonth(date2))
				{
					d2 = 30;
				}

				auto day_count = days{
					360 * (y2 - y1) + 30 * (m2 - m1) + (d2 - d1)
				};

				auto term = day_count.count() / 360.0;

				return { day_count, term };
			}

			case EDayCount::d30_d365:
			{
				auto [d1, m1, y1] = decompose(date1);
				auto [d2, m2, y2] = decompose(date2);

				auto days_in_period = days{
					365 * (y2 - y1) + 30 * (m2 - m1) + (d2 - d1)
				};
				auto term = days_in_period.count() / 365.0;
				return { days_in_period, term };
			}

            case EDayCount::Actual_Actual:
			case EDayCount::Actual_Actual_ISDA:
				{
					auto firstAccrualDate = year_month_day{date1.year() / date1.month() / date1.day()};
                    auto daysInPeriod = days{0};
                    auto term = 0.0;

					// Get the period from now to the end of the year
					if (firstAccrualDate.year() < date2.year())
					{
						daysInPeriod = days{1} + (daysInYear(firstAccrualDate.year()) - dayOfYear(firstAccrualDate));
						term = daysInPeriod.count() / (double)daysInYear(firstAccrualDate.year()).count();

						firstAccrualDate = year_month_day((firstAccrualDate.year() + years{1}) / January / 1d);

						// Get the whole years from the moved start to the end
						while (firstAccrualDate.year() < date2.year())
						{
							term += 1;
							daysInPeriod += daysInYear(firstAccrualDate.year());
							firstAccrualDate = {(firstAccrualDate.year() + years{1}) / January / 1d};
						}
						// Years should now be the same.
					}
					else
					{
						term = 0;
						daysInPeriod = days{0};
					}

					// Handle the end stub.
					auto end_days = (dayOfYear(date2) - dayOfYear(firstAccrualDate));
					term += end_days.count() / (double)daysInYear(firstAccrualDate.year()).count();
					daysInPeriod += end_days;
                    return { daysInPeriod, term };
				}

            case EDayCount::Actual_Actual_ISMA:
				{
                    auto daysInPeriod = days{0};
                    auto term = 0.0;

                    if (date1.year() == date2.year())
                    {
                        // If we are in the same year this is a simple case.
                        daysInPeriod = sys_days{date2} - sys_days{date1};
                        term = daysInPeriod.count() / (double)daysInYear(date1.year()).count();
                    }
                    else
                    {
                        // Calculate the term from the start date to the end of the year.
                        daysInPeriod = days{1} + daysInYear(date1.year()) - dayOfYear(date1);
                        term = daysInPeriod.count() / static_cast<double>(daysInYear(date1.year()).count());

                        if (addMonths(date1, months{12}, true) > date2)
                        {
                            // The dates are less than a year apart, but are in different years.
                            // Simply add on the year fraction for the end stub.
							auto period_days = dayOfYear(date2) - days{1};
                            term += period_days.count() / static_cast<double>(daysInYear(date2.year()).count());
                            daysInPeriod += period_days;
                            return { daysInPeriod, term };
                        }
                        else
                        {
                            // Find a date in the next year, which has the same day and month as the end date.
                            auto y = date1.year() + years{1};
                            auto m = date2.month();
                            auto d = std::min(date2.day(), lastDayOfMonth(date1.year() + years{1}, date2.month()));
							auto fcd = year_month_day(y / m / d);
                            // Add on the year fraction for this stub.
							auto period_days = dayOfYear(fcd) - days{1};
                            term += period_days.count() / static_cast<double>(daysInYear(fcd.year()).count());
                            daysInPeriod += period_days;

                            // The remaining time is a whole number of years.
                            while (fcd < date2)
                            {
                                term += 1;
                                daysInPeriod += daysInYear(fcd.year());
                                fcd = addMonths(fcd, months{12}, true);
                            }
                            return { daysInPeriod, term };
                        }
                    }
                }

			case EDayCount::Actual_Actual_AFB:
				{
                    if (date1.year() == date2.year())
                    {
                        // If we are in the same year this is a simple case.
						auto period_days = (sys_days{date2} - sys_days{date1});
                        auto term = period_days.count() / (double)daysInYear(date1.year()).count();
                        return { period_days, term };
                    }
                    else
                    {
                        // If either this year of the next year is a leap year, use 366 as the number of days in the year.
                        auto days_in_year = days{date1.year().is_leap() || (date1.year() + years{1}).is_leap() ? 366 : 365};

                        // Calculate the number of days left in the year.
                        auto days_in_period = (days{1} + daysInYear(date1.year()) - dayOfYear(date1));
                        auto term = 0.0;

                        if (addMonths(date1, months{12}, true) > date2)
                        {
                            // Since the denominator is fixed we can accumulate the number of days and maintain precision.
                            days_in_period += dayOfYear(date2) - days{1};
                            term = days_in_period.count() / static_cast<double>(days_in_year.count());
                            return { days_in_period, term };
                        }
                        else
                        {
                            // Find a date in the next year, which has the same day and month as the end date.
                            auto y = date1.year() + years{1};
                            auto m = date2.month();
                            auto d = std::min(date2.day(), lastDayOfMonth(date1.year() + years{1}, date2.month()));
							auto fcd = year_month_day{y / m / d};

                            // Since the denominator is fixed we can accumulate the number of days and maintain precision.
                            days_in_period += dayOfYear(fcd) - days{1};
                            term = days_in_period.count() / static_cast<double>(days_in_year.count());

                            // The remaining time is a whole number of years.
                            while (fcd < date2)
                            {
                                term += 1;
                                days_in_period += daysInYear(fcd.year());
                                fcd = addMonths(fcd, months{12}, true);
                            }

                            return { days_in_period, term };
                        }
                    }
				}

            default:
				throw std::invalid_argument("invalid daycount");
		}
	}

	inline double
	yearFrac(const year_month_day& start, const year_month_day& end, EDayCount dayCount)
	{
		const auto& [d, t] = getTerm(start, end, dayCount);
		return t;
	}


	inline double
	yearsBetween(const year_month_day& start, const year_month_day& end, EDayCount dayCount)
	{
		const auto& [d, t] = getTerm(start, end, dayCount);
		return t;
	}

	inline days
	daysBetween(const year_month_day& start, const year_month_day& end, EDayCount dayCount)
	{
		const auto& [d, t] = getTerm(start, end, dayCount);
		return d;
	}

	// <summary>
	// Calculates the year fraction between a start date and an schedule of N dates, returning
	// an array fo year fractions of size N-1.
	// </summary>
	// <param name="firstAccrualDate">The start date</param>
	// <param name="schedule">The end date</param>
	// <param name="daycount">The daycount convention</param>
	// <returns>
	// The schedule of year fractions
	// </returns>
	inline
	std::vector<double>
	yearsBetween(const year_month_day& firstAccrualDate, const std::vector<year_month_day> schedule, EDayCount dayCount)
	{
		std::vector<double> terms;
		for (const auto& date  : schedule)
			terms.push_back(yearsBetween(firstAccrualDate, date, dayCount));
		return terms;
	}

	// <summary>
	// Calculates the year fraction between a the first date and subsequent dates in a schedule
	// of N dates, returning N-2 year fractions.
	// </summary>
	// <param name="firstAccrualDate">The start date</param>
	// <param name="schedule">The end date</param>
	// <param name="daycount">The daycount convention</param>
	// <returns>
	// The schedule of year fractions
	// </returns>
	inline
	std::vector<double>
	yearsBetween(const std::vector<year_month_day> schedule, EDayCount dayCount)
	{
		std::vector<double> terms;
		if (schedule.size() > 0)
		{
			std::vector<year_month_day>::const_iterator i(schedule.begin());
			const year_month_day& start_date = *i;
			while (++i != schedule.end())
			{
				const year_month_day& coupon_date = *i;
				double t = yearsBetween(start_date, coupon_date, dayCount);
				terms.push_back(t);
			}
		}
		return terms;
	}
}

static const struct { const char* string_type; dates::EDayCount enum_type; } Daycount_TypeMap[] =
{
	{"ACT/360",		dates::EDayCount::Actual_d360},
	{"ACT/365",		dates::EDayCount::Actual_d365},
	{"ACT/366",		dates::EDayCount::Actual_d366},
	{"ACT/365.25",	dates::EDayCount::Actual_d365_25},
	{"ACT/ACT",		dates::EDayCount::Actual_Actual},
	{"NY/365",		dates::EDayCount::NY_d365},
	{"30/360",		dates::EDayCount::d30A_360},
	{"30/365",		dates::EDayCount::d30_d365},
	{"30E/360",		dates::EDayCount::d30E_d360},
	{"Act/ActISDA",	dates::EDayCount::Actual_Actual_ISDA},
	{"Act/ActISMA",	dates::EDayCount::Actual_Actual_ISMA},
	{"Act/ActAFB",	dates::EDayCount::Actual_Actual_AFB}
};

inline dates::EDayCount& operator>>(const std::string& lhs, dates::EDayCount& rhs)
{
	for (size_t i = 0; i < sizeof(Daycount_TypeMap) / sizeof(Daycount_TypeMap[0]); ++i)
		if (std::strcmp(lhs.c_str(), Daycount_TypeMap[i].string_type) == 0)
			return rhs = Daycount_TypeMap[i].enum_type;

	throw std::invalid_argument("unknown enum");
}

inline std::string& operator<<(std::string& lhs, const dates::EDayCount& rhs)
{
	for (size_t i = 0; i < sizeof(Daycount_TypeMap) / sizeof(Daycount_TypeMap[0]); ++i)
		if (rhs == Daycount_TypeMap[i].enum_type)
			return lhs = Daycount_TypeMap[i].string_type;

	throw std::invalid_argument("unknown enum");
}


inline std::ostream& operator<<(std::ostream& lhs, const dates::EDayCount& rhs)
{
	std::string s;
	s << rhs;
	lhs << s;
	return lhs;
}

#endif // __jetblack__dates__terms_hpp
