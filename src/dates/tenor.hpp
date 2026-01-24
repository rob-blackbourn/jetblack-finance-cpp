#ifndef __jetblack__dates__tenor_hpp
#define __jetblack__dates__tenor_hpp

#include <cstring>

#include <chrono>
#include <set>
#include <string>
#include <vector>

#include "dates/arithmetic.hpp"
#include "dates/business_days.hpp"
#include "dates/adjustments.hpp"

namespace dates
{
	struct Tenor
	{
		std::chrono::months	months;
		std::chrono::weeks	weeks;
		std::chrono::days	days;
		std::chrono::days	spot_days;

		Tenor(
			std::chrono::months months = std::chrono::months{0},
			std::chrono::weeks weeks = std::chrono::weeks{0},
			std::chrono::days days = std::chrono::days{0},
			std::chrono::days spot_days = std::chrono::days{0})
			: months(months), weeks(weeks), days(days), spot_days(spot_days)
		{
		}

		Tenor(EFrequency frequency, std::chrono::days spot_days = std::chrono::days{0})
			: spot_days(spot_days)
		{
			switch (frequency)
			{
			case EFrequency::Annual:
				days = std::chrono::days{0};
				weeks = std::chrono::weeks{0};
				months = std::chrono::months{12};
				break;
			case EFrequency::SemiAnnual:
				days = std::chrono::days{0};
				weeks = std::chrono::weeks{0};
				months = std::chrono::months{6};
				break;
			case EFrequency::Quarterly:
				days = std::chrono::days{0};
				weeks = std::chrono::weeks{0};
				months = std::chrono::months{3};
				break;
			case EFrequency::Monthly:
				days = std::chrono::days{0};
				weeks = std::chrono::weeks{0};
				months = std::chrono::months{1};
				break;
			case EFrequency::Weekly:
				days = std::chrono::days{0};
				weeks = std::chrono::weeks{1};
				months = std::chrono::months{0};
				break;
			case EFrequency::Daily:
				days = std::chrono::days{1};
				weeks = std::chrono::weeks{0};
				months = std::chrono::months{0};
				break;
			case EFrequency::None:
				days = std::chrono::days{0};
				weeks = std::chrono::weeks{0};
				months = std::chrono::months{0};
				break;
			default:
				throw "unhandled frequency";
			}
		}

		Tenor(const Tenor& tenor)
			: months(tenor.months), weeks(tenor.weeks), days(tenor.days), spot_days(tenor.spot_days)
		{
		}

		Tenor(const char* tenor_code, const std::chrono::days& spot_days = std::chrono::days{0})
		{
			if (tenor_code == NULL || strlen(tenor_code) == 0)
				throw "empty tenor";
			else if (std::strcmp("S", tenor_code) == 0)
			{
				days = std::chrono::days{0};
				weeks = std::chrono::weeks{0};
				months = std::chrono::months{0};
				this->spot_days = spot_days;
			}
			else if (std::strcmp("ON", tenor_code) == 0)
			{
				this->spot_days = std::chrono::days{0};
				days = std::chrono::days{1};
				weeks = std::chrono::weeks{0};
				months = std::chrono::months{0};
			}
			else if (std::strcmp("TN", tenor_code) == 0)
			{
				this->spot_days = std::chrono::days{1};
				days = std::chrono::days{1};
				weeks = std::chrono::weeks{0};
				months = std::chrono::months{0};
			}
			else if (std::strcmp("SN", tenor_code) == 0)
			{
				this->spot_days = spot_days;
				days = std::chrono::days{1};
				weeks = std::chrono::weeks{0};
				months = std::chrono::months{0};
			}
			else if (std::strcmp("SW", tenor_code) == 0)
			{
				this->spot_days = spot_days;
				days = std::chrono::days{0};
				weeks = std::chrono::weeks{1};
				months = std::chrono::months{0};
			}
			else
			{
				// Follows the format <a>Y<b>M<c>W<d>D where a, b, and c are whole numbers
				// and the characters are units corresponding to Y=Year, M=Month, W=Week,
				// and D=Day.
				//
				// The number-unit pairs can be in any order.
				//
				// The last unit can be omitted (e.g. 1Y6). The applied unit depends on the
				// unit of the immediately preceding pair. The chosen unit follows these
				// rules: Y->M, M->D, W->D. So the above example becomes 1Y6M.
				//
				// It is an error for any one unit to appear more than once.
				// It is an error to use an unknown tenor.

				int i, error = 0;
				int count[4];
				char units[4];

				// 2Y3M1W2D
				int matches =
					sscanf(
						tenor_code,
						"%d%c%d%c%d%c%d%c",
						&count[0], &units[0],
						&count[1], &units[1],
						&count[2], &units[2],
						&count[3], &units[3]);

				if (matches < 2)
					throw "Invalid date";

				int lastfullmatch = matches / 2 - 1;
				int lastpartialmatch = matches / 2 + matches % 2 - 1;

				// Replace unspecified cells with "0X".
				for (i = lastpartialmatch+1; i < 4; ++i)
					count[i] = 0, units[i] = 'X';

				// Complete any partial match.
				if (lastfullmatch < lastpartialmatch)
				{
					switch (units[lastfullmatch])
					{
					case 'Y':
						units[lastpartialmatch] = 'M';
						break;
					case 'M':
						units[lastpartialmatch] = 'D';
						break;
					case 'W':
						units[lastpartialmatch] = 'D';
						break;
					default:
						++error;
						break;
					}
				}

				// Get the date offset in terms of days and months.
				days = std::chrono::days{0};
				weeks = std::chrono::weeks{0};
				months = std::chrono::months{0};
				int d = 0, w = 0, m = 0, y = 0;
				for (i = 0; i < sizeof(units) / sizeof(units[0]); ++i)
				{
					switch (units[i])
					{
					case 'Y':
						++y;
						months += std::chrono::months{12 * count[i]};
						break;
					case 'M':
						++m;
						months += std::chrono::months{count[i]};
						break;
					case 'W':
						++w;
						weeks += std::chrono::weeks{count[i]};
						break;
					case 'D':
						++d;
						days += std::chrono::days{count[i]};
						break;
					case 'X':
						break;
					default:
						++error;
						break;
					}
				}

				// How did we do?
				error += (d > 1) + (w > 1) + (m > 1) + (y > 1);

				if (error > 0)
					throw "Invalid date";

				this->spot_days = (days == std::chrono::days{0} && weeks == std::chrono::weeks{0} ? spot_days : std::chrono::days{0});
			}
		}

		year_month_day spot_date(
			const year_month_day& effective_date,
			EDateRule date_rule,
			const std::set<year_month_day>& holidays = std::set<year_month_day>()) const
		{
			auto d = year_month_day{effective_date};
			if (spot_days > std::chrono::days{0})
				d = addBusinessDays(d, spot_days, holidays);

			return d;
		}

		year_month_day maturity_date(const year_month_day& effective_date, EDateRule date_rule, const std::set<year_month_day>& holidays = std::set<year_month_day>()) const
		{
			auto d = spot_date(effective_date, date_rule, holidays);
			if (days > std::chrono::days{0})
				d = dates::addBusinessDays(d, days, holidays);
			d = sys_days{d} + weeks;
			d = addMonths(d, months, true);
			d = adjust(d, date_rule, holidays);

			return d;
		}

		bool operator < (const Tenor& rhs) const
		{
			auto d1 = spot_days + days + weeks + std::chrono::days{months.count() * 30};
			auto d2 = spot_days + days + weeks + std::chrono::days{months.count() * 30};
			return d1 < d2;
		}

		bool operator == (const Tenor& rhs) const
		{
			return
					spot_days == rhs.spot_days
					&& days == rhs.days
					&& weeks == rhs.weeks
					&& months == rhs.months;
		}

		Tenor operator - () const
		{
			return Tenor(-months, -weeks, -days, -spot_days);
		}

		Tenor operator * (int i) const
		{
			return Tenor(months * i, weeks * i, days * i, spot_days);
		}
	};

	inline
	year_month_day
	add(
		const year_month_day& date,
		const Tenor& tenor,
		EDateRule dateRule = EDateRule::Following,
		const std::set<year_month_day>& holidays = std::set<year_month_day>())
    {
		auto d = date;

        if (tenor.days != std::chrono::days{0})
            d = addBusinessDays(d, tenor.days, holidays);

        if (tenor.weeks != std::chrono::weeks{0})
        {
            d = sys_days{d} + tenor.weeks;
            d = adjust(d, EDateRule::Following, holidays);
        }

        if (tenor.months != std::chrono::months{0})
        {
            d = addMonths(d, tenor.months, true);
			d = adjust(d, dateRule, holidays);
        }

        return d;
    }
}

inline
std::string& operator<<(std::string& lhs, const dates::Tenor& rhs)
{
	int years = rhs.months.count() / 12;
	int months = rhs.months.count() % 12;
	
	std::stringstream ss;

	if (rhs.spot_days != std::chrono::days{0} && rhs.days == std::chrono::days{0} && rhs.weeks == std::chrono::weeks{0} && rhs.months == std::chrono::months{0})
	{
		lhs = "S";
	}
	else if (rhs.spot_days != std::chrono::days{0} && rhs.days == std::chrono::days{0} && rhs.weeks == std::chrono::weeks{1} && rhs.months == std::chrono::months{0})
	{
		lhs = "SW";
	}
	else if (rhs.spot_days == std::chrono::days{0} && rhs.days == std::chrono::days{1} && rhs.weeks == std::chrono::weeks{0} && rhs.months == std::chrono::months{0})
	{
		lhs = "ON";
	}
	else if (rhs.spot_days == std::chrono::days{1} && rhs.days == std::chrono::days{1} && rhs.weeks == std::chrono::weeks{0} && rhs.months == std::chrono::months{0})
	{
		lhs = "TN";
	}
	else if (rhs.spot_days > std::chrono::days{1} && rhs.days == std::chrono::days{1} && rhs.weeks == std::chrono::weeks{0} && rhs.months == std::chrono::months{0})
	{
		lhs = "SN";
	}
	else
	{
		if (years > 0)
			ss << years << 'Y';
		if (months > 0)
			ss << months << 'M';
		if (rhs.weeks > std::chrono::weeks{0})
			ss << rhs.weeks.count() << 'W';
		if (rhs.days > std::chrono::days{0})
			ss << rhs.days.count() << 'D';

		lhs = ss.str();
	}

	return lhs;
}

inline std::ostream& operator<<(std::ostream& lhs, const dates::Tenor& rhs)
{
	std::string s;
	s << rhs;
	lhs << s;
	return lhs;
}

#endif // __jetblack__dates__tenor_hpp
