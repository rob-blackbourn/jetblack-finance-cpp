#ifndef __jetblack__dates__terms_hpp
#define __jetblack__dates__terms_hpp

#include <cstring>

#include <algorithm>
#include <chrono>
#include <set>
#include <string>
#include <type_traits>
#include <vector>

#include "dates/arithmetic.hpp"

namespace dates
{
    using namespace std::chrono;

	enum class EDayCount
	{ 
		Actual_d360 = 0,
		Actual_d365 = 1,
		Actual_d366 = 2,
		Actual_d365_25 = 3,
		Actual_Actual = 4,
		NY_d365 = 5,
		d30_d360 = 6,
		d30_d365 = 7,
		d30E_d360 = 8,
		Actual_Actual_ISDA = 9,
		Actual_Actual_ISMA = 10,
		Actual_Actual_AFB = 11
	};

	// <summary>
	// Calculates the year fraction between two dates for a given daycount convention.
	// </summary>
	// <param name="start">The start date</param>
	// <param name="end">The end date</param>
	// <param name="daycount">The daycount convention</param>
	// <returns>
	// The days and the year fraction
	// </returns>
	inline
	std::pair<days,double>
	getTerm(const year_month_day& start, const year_month_day& end, EDayCount dayCount)
	{
		// double days, term;

		switch(dayCount)
		{
			case EDayCount::Actual_d360:
				{
					auto days_in_period = (sys_days{end} - sys_days{start});
                    auto days_in_year = 360.0;
					auto term = days_in_period.count() / days_in_year;
                    return { days_in_period, term };
				}
			case EDayCount::Actual_d365:
				{
					auto days_in_period = (sys_days{end} - sys_days{start});
                    auto days_in_year = 365.0;
					auto term = days_in_period.count() / days_in_year;
                    return { days_in_period, term };
				}
			case EDayCount::Actual_d366:
				{
					auto days_in_period = (sys_days{end} - sys_days{start});
                    auto days_in_year = 366.0;
					auto term = days_in_period.count() / days_in_year;
                    return { days_in_period, term };
				}
			case EDayCount::Actual_d365_25:
				{
					auto days_in_period = (sys_days{end} - sys_days{start});
                    auto days_in_year = 365.25;
					auto term = days_in_period.count() / days_in_year;
                    return { days_in_period, term };
				}

			case EDayCount::d30_d360:
			{
				auto d1 = start.day(), d2 = end.day();
				auto m1 = start.month(), m2 = end.month();
				auto y1 = start.year(), y2 = end.year();

				auto days_in_year = 360.0;

				if (false)
				{
					// Method 1
					if (d1 == 31d)
					{
						d1 = 30d;
					}

					if (d2 == 31d && d1 == 30d)
					{
						d2 = 30d;
					}

					auto days_in_period = days{(360 * (y2 - y1).count() + 30 * (m2 - m1).count() + (d2 - d1).count())};
					auto term = days_in_period.count() / days_in_year;
					return { days_in_period, term };
				}
				else
				{
					// Method 2
					if (d2 == 31d && d1 < 30d)
					{
						d2 = 1d;
						m2++;
					}

					auto days_in_period = days{360 * (y2 - y1).count() + 30 * ((m2 - m1).count() - 1) + std::max(0, (30d - d1).count()) + static_cast<unsigned int>(std::min(30d, d2))};
					auto term = days_in_period.count() / 360.0;
					return { days_in_period, term };
				}
			}

			case EDayCount::d30_d365:
			{
				auto d1 = start.day(), d2 = end.day();
				auto m1 = start.month(), m2 = end.month();
				year y1 = start.year(), y2 = end.year();

				auto days_in_period = days{(365 * (y2 - y1).count() + 30 * (m2 - m1).count() + (d2 - d1).count())};
				auto term = days_in_period.count() / 365.0;
				return { days_in_period, term };
			}

			case EDayCount::d30E_d360:
			{
				auto d1 = start.day(), d2 = end.day();
				auto m1 = start.month(), m2 = end.month();
				auto y1 = start.year(), y2 = end.year();

				if (false)
				{
					// method 1
					if ((d2 == 31d && d1 < 30d) || (m2 == February && isEndOfMonth(end)))
					{
						d2 = 30d;
					}

					auto days_in_period = days{(360 * (y2 - y1).count() + 30 * (m2 - m1).count() + (d2 - d1).count())};
					auto term = days_in_period.count() / 360.0;
					return { days_in_period, term };
				}
				else
				{
					// method 2
					auto days_in_period = days{360 * (y2 - y1).count() + 30 * ((m2 - m1).count() - 1) + std::max(0, (30d - d1).count()) + static_cast<unsigned int>(std::min(30d, d2))};
					auto term = days_in_period.count() / 360.0;
					return { days_in_period, term };
				}
			}

            case EDayCount::Actual_Actual:
			case EDayCount::Actual_Actual_ISDA:
				{
					auto firstAccrualDate = year_month_day{start.year() / start.month() / start.day()};
                    auto daysInPeriod = days{0};
                    auto term = 0.0;

					// Get the period from now to the end of the year
					if (firstAccrualDate.year() < end.year())
					{
						daysInPeriod = days{1} + (daysInYear(firstAccrualDate.year()) - dayOfYear(firstAccrualDate));
						term = daysInPeriod.count() / (double)daysInYear(firstAccrualDate.year()).count();

						firstAccrualDate = year_month_day((firstAccrualDate.year() + years{1}) / January / 1d);

						// Get the whole years from the moved start to the end
						while (firstAccrualDate.year() < end.year())
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
					auto end_days = (dayOfYear(end) - dayOfYear(firstAccrualDate));
					term += end_days.count() / (double)daysInYear(firstAccrualDate.year()).count();
					daysInPeriod += end_days;
                    return { daysInPeriod, term };
				}

            case EDayCount::Actual_Actual_ISMA:
				{
                    auto daysInPeriod = days{0};
                    auto term = 0.0;

                    if (start.year() == end.year())
                    {
                        // If we are in the same year this is a simple case.
                        daysInPeriod = sys_days{end} - sys_days{start};
                        term = daysInPeriod.count() / (double)daysInYear(start.year()).count();
                    }
                    else
                    {
                        // Calculate the term from the start date to the end of the year.
                        daysInPeriod = days{1} + daysInYear(start.year()) - dayOfYear(start);
                        term = daysInPeriod.count() / static_cast<double>(daysInYear(start.year()).count());

                        if (addMonths(start, months{12}, true) > end)
                        {
                            // The dates are less than a year apart, but are in different years.
                            // Simply add on the year fraction for the end stub.
							auto period_days = dayOfYear(end) - days{1};
                            term += period_days.count() / static_cast<double>(daysInYear(end.year()).count());
                            daysInPeriod += period_days;
                            return { daysInPeriod, term };
                        }
                        else
                        {
                            // Find a date in the next year, which has the same day and month as the end date.
                            auto y = start.year() + years{1};
                            auto m = end.month();
                            auto d = std::min(end.day(), lastDayOfMonth(start.year() + years{1}, end.month()));
							auto fcd = year_month_day(y / m / d);
                            // Add on the year fraction for this stub.
							auto period_days = dayOfYear(fcd) - days{1};
                            term += period_days.count() / static_cast<double>(daysInYear(fcd.year()).count());
                            daysInPeriod += period_days;

                            // The remaining time is a whole number of years.
                            while (fcd < end)
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
                    if (start.year() == end.year())
                    {
                        // If we are in the same year this is a simple case.
						auto period_days = (sys_days{end} - sys_days{start});
                        auto term = period_days.count() / (double)daysInYear(start.year()).count();
                        return { period_days, term };
                    }
                    else
                    {
                        // If either this year of the next year is a leap year, use 366 as the number of days in the year.
                        auto days_in_year = days{start.year().is_leap() || (start.year() + years{1}).is_leap() ? 366 : 365};

                        // Calculate the number of days left in the year.
                        auto days_in_period = (days{1} + daysInYear(start.year()) - dayOfYear(start));
                        auto term = 0.0;

                        if (addMonths(start, months{12}, true) > end)
                        {
                            // Since the denominator is fixed we can accumulate the number of days and maintain precision.
                            days_in_period += dayOfYear(end) - days{1};
                            term = days_in_period.count() / static_cast<double>(days_in_year.count());
                            return { days_in_period, term };
                        }
                        else
                        {
                            // Find a date in the next year, which has the same day and month as the end date.
                            auto y = start.year() + years{1};
                            auto m = end.month();
                            auto d = std::min(end.day(), lastDayOfMonth(start.year() + years{1}, end.month()));
							auto fcd = year_month_day{y / m / d};

                            // Since the denominator is fixed we can accumulate the number of days and maintain precision.
                            days_in_period += dayOfYear(fcd) - days{1};
                            term = days_in_period.count() / static_cast<double>(days_in_year.count());

                            // The remaining time is a whole number of years.
                            while (fcd < end)
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
		return getTerm(start, end, dayCount).second;
	}


	inline double
	yearsBetween(const year_month_day& start, const year_month_day& end, EDayCount dayCount)
	{
		return getTerm(start, end, dayCount).second;
	}

	inline days
	daysBetween(const year_month_day& start, const year_month_day& end, EDayCount dayCount)
	{
		return getTerm(start, end, dayCount).first;
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
	{"30/360",		dates::EDayCount::d30_d360},
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
