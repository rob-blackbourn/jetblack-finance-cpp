#ifndef __jetblack__dates__tenor_schedules_hpp
#define __jetblack__dates__tenor_schedules_hpp

#include <cstring>

#include <algorithm>
#include <chrono>
#include <set>
#include <string>
#include <type_traits>
#include <vector>

#include "dates/arithmetic.hpp"
#include "dates/schedules.hpp"
#include "dates/tenor.hpp"

namespace dates
{
    using namespace std::chrono;



	// <summary>
	// Returns the number of days between two dates after removing the number of complete months.
	// </summary>
	// <param name="firstAccrualDate">The start date</param>
	// <param name="endDate">The end date</param>
	// <returns>
	// The number of days between two dates after removing the number of complete months.
	// </returns>
	inline
	days
	monthMod(const year_month_day& firstAccrualDate, const year_month_day& endDate)
	{
		if (firstAccrualDate.day() == endDate.day())
			return days {0};
		else if (firstAccrualDate.day() < endDate.day())
			return endDate.day() - firstAccrualDate.day();
		else
		{
			auto last_day = lastDayOfMonth(firstAccrualDate.year(), firstAccrualDate.month());
			auto offset = last_day - firstAccrualDate.day();
			auto daysFromOffset = offset + endDate.day();
			return days {static_cast<unsigned int>(daysFromOffset)};
		}
	}

	// <summary>
	// Returns the number of whole months between two dates.
	// </summary>
	// <param name="firstAccrualDate">The start date</param>
	// <param name="endDate">The end date</param>
	// <param name="EOMFlag">A boolean flag indicating whether intermediate dates are assumed to lie at the end of the month</param>
	// <returns>
	// The number of whole months between two dates.
	// </returns>
	inline
	months
	monthDiff(const year_month_day& firstAccrualDate, const year_month_day& endDate, bool EOMFlag)
	{
		year_month_day start, end;

		if (EOMFlag)
		{
			start = year_month_day {firstAccrualDate.year(), firstAccrualDate.month(), 1d };
			end = year_month_day {endDate.year(), endDate.month(), 1d};
		}
		else
		{
			start = sys_days(firstAccrualDate) + monthMod(firstAccrualDate, endDate);
			end = endDate;
		}

		months months = (end.year() - start.year()) * 12;
		months += end.month() - start.month();

		return months;
	}

	inline
	std::vector<year_month_day>
	generateScheduleNoCheck(const year_month_day& firstAccrualDate, const year_month_day& firstRegularDate, const year_month_day& lastRegularDate, const year_month_day& endDate, const day& dayOfMonth, const months& periodMonths)
	{
		std::vector<year_month_day> dates;

		if (firstAccrualDate < firstRegularDate)
			dates.push_back(firstAccrualDate);

		year_month_day date = firstRegularDate;
		while (date <= lastRegularDate)
		{
			dates.push_back(date);

			date = year_month_day(date.year(), date.month(), 1d);
			date = addMonths(date, periodMonths, true);
			if (dayOfMonth == 0d)
				date = moveToEndOfMonth(date);
			else
				date = year_month_day(
					date.year(),
					date.month(),
					std::min(dayOfMonth, lastDayOfMonth(date.year(), date.month())));
		}

		if (endDate > lastRegularDate)
			dates.push_back(endDate);

		return dates;
	}

	// <summary>
	// This function generates a schedule of dates between the statr date and the end date
	// for a given frequency. The position of any non-whole period is controlled by the odd-at-start
	// flag. The end of month flag can be used to specify that the given dates are intended to
	// always lie at the end of the month.
	// </summary>
	// <param name="firstAccrualDate">The start date</param>
	// <param name="endDate">The end date</param>
	// <param name="eom">A boolean flag indicating whether intermediate dates are assumed to lie at the end of the month</param>
	// <param name="frequency">The frequency of the schedule</param>
	// <param name="oddAtStart">If true any non-whole period will be placed at the start of the schedule, else it will be placed at the end</param>
	// <returns>A schedule of dates</returns>
	inline
	std::vector<year_month_day>
	generateSchedule(
		const year_month_day& firstAccrualDate,
		const year_month_day& endDate,
		bool eom,
		EFrequency frequency,
		bool oddAtStart)
	{
		if (!(frequency == EFrequency::Annual || frequency == EFrequency::SemiAnnual || frequency == EFrequency::Quarterly || frequency == EFrequency::Monthly))
			throw std::invalid_argument("The frequency must be annual, semi-annual, quarterly or monthly");

		year_month_day start, end;
		if (eom)
		{
			// move all dates to the end of the month
			start = moveToEndOfMonth(firstAccrualDate);
			end = moveToEndOfMonth(endDate);
		}
		else
		{
			start = firstAccrualDate;
			end = endDate;
		}

		// Checck the date order is valid
		if (firstAccrualDate > endDate)
			throw std::invalid_argument("The start date must be on or before the end date");

		// Discover the first regular date
		auto period_months = months{12 / std::to_underlying(frequency)};
		// The absolute number of months between the two years.
		auto duration_months = monthDiff(start, end, eom);
		// Odd months is the remainder that do not fit in a whole period.
		auto odd_months = duration_months % period_months;
		// Odd days are the days between the whole month dates
		auto odd_days = monthMod(start, end);

		// Discover the regular dates
		year_month_day firstRegular, lastRegular;
		day dayOfMonth;
		if (oddAtStart)
		{
			lastRegular = end;

			firstRegular = start;
			firstRegular = sys_days{firstRegular} + odd_days;
			firstRegular = addMonths(firstRegular, odd_months, true);

			if (eom)
				dayOfMonth = 0d;
			else
			{
				dayOfMonth = lastRegular.day();
				firstRegular =
					year_month_day(
						firstRegular.year(),
						firstRegular.month(),
						std::min(dayOfMonth, lastDayOfMonth(firstRegular.year(), firstRegular.month())));
			}
		}
		else
		{
			firstRegular = start;

			lastRegular = end;
			lastRegular = sys_days{lastRegular} - odd_days;
			lastRegular = addMonths(lastRegular, -odd_months, true);

			if (eom)
				dayOfMonth = 0d;
			else
				dayOfMonth = firstRegular.day();
		}

		return generateScheduleNoCheck(start, firstRegular, lastRegular, end, dayOfMonth, period_months);
	}


	// <summary>
	// This function generates a schedule with: one period between the start date and
	// first regular date, and regular schedule between the first and last regular
	// date, and one period bewteen the last regular date and the end date.
	// </summary>
	// <param name="firstAccrualDate">The start date</param>
	// <param name="firstRegularDate">The fist regular date</param>
	// <param name="lastRegularDate">The last regular date</param>
	// <param name="endDate">The end date</param>
	// <param name="eom">A boolean flag indicating whether intermediate dates are assumed to lie at the end of the month</param>
	// <param name="frequency">The frequency of the schedule</param>
	// <returns>The schedule of dates</returns>
	inline
	std::vector<year_month_day>
	generateSchedule(
		const year_month_day& firstAccrualDate,
		const year_month_day& firstRegularDate,
		const year_month_day& lastRegularDate,
		const year_month_day& endDate,
		bool eom,
		EFrequency frequency)
	{
		if (!(frequency == EFrequency::Annual || frequency == EFrequency::SemiAnnual || frequency == EFrequency::Quarterly || frequency == EFrequency::Monthly))
			throw std::invalid_argument("The frequency must be annual, semi-annual, quarterly or monthly");

		year_month_day start, firstRegular, lastRegular, end;
		day dayOfMonth;
		if (eom)
		{
			// move all dates to the end of the month
			start = moveToEndOfMonth(firstAccrualDate);
			firstRegular = moveToEndOfMonth(firstRegularDate);
			lastRegular = moveToEndOfMonth(lastRegularDate);
			end = moveToEndOfMonth(endDate);
			dayOfMonth = 0d;
		}
		else
		{
			auto firstRegularDayOfMonth = firstRegularDate.day();
			auto firstRegularDaysInMonth = lastDayOfMonth(firstRegularDate.year(), firstRegularDate.month());
			auto lastRegularDayOfMonth = lastRegularDate.day();
			auto lastRegularDaysInMonth = lastDayOfMonth(lastRegularDate.year(), lastRegularDate.month());

			// Check the days of the months match for the supposedly regular dates
			if ( ! (
					(firstRegularDayOfMonth == lastRegularDayOfMonth)
					|| (firstRegularDayOfMonth == firstRegularDaysInMonth && firstRegularDayOfMonth < lastRegularDayOfMonth)
					|| (lastRegularDayOfMonth == lastRegularDaysInMonth && lastRegularDayOfMonth < firstRegularDayOfMonth) ) )
				throw std::invalid_argument("first regular date and last regular date must correspond");
			start = firstAccrualDate;
			firstRegular = firstRegularDate;
			lastRegular = lastRegularDate;
			end = endDate;
			dayOfMonth = std::max(firstRegular.day(), lastRegular.day());
		}

		// Checck the date order is valid
		if (firstAccrualDate > firstRegularDate)
			throw std::invalid_argument("The start date must be on or before the first regular date");
		else if (firstRegularDate > lastRegularDate)
			throw std::invalid_argument("The first regular date must be on or before the last regular date");
		else if (lastRegularDate > endDate)
			throw std::invalid_argument("The last regular date must be on or before the end date");

		// Check we have a whole number of periods between the regular dates
		auto period_months = months{12 / std::to_underlying(frequency)};
		auto duration_months = monthDiff(firstRegular, lastRegular, eom);
		if (duration_months % period_months != months{0})
			throw std::invalid_argument("The months between the regular dates is not a whole number of periods");

		if (!eom)
		{
			auto remainingDays = monthMod(firstRegular, lastRegular);
			if (remainingDays != days{0})
				throw std::invalid_argument("The number of days between the first regular day-of-month and the last regular day-of-month must be 0. ");
		}

		return generateScheduleNoCheck(start, firstRegular, lastRegular, end, dayOfMonth, period_months);
	}

	// <summary>
	// This function adjusts a schedule according to a given business rule and holiday list.
	// </summary>
	// <param name="dates">The schedule of dates</param>
	// <param name="dateRule">The adjustment rule</param>
	// <param name="holidays">A list of holidays</param>
	// <returns>An adjusted schedule of dates</returns>
	inline
	std::vector<year_month_day>
	adjustSchedule(const std::vector<year_month_day>& dates, EDateRule dateRule, const std::set<year_month_day>& holidays)
	{
		std::vector<year_month_day> adjustedDates;
		for (const auto& date : dates)
			adjustedDates.push_back(adjust(date, dateRule, holidays));
		return adjustedDates;
	}

	// <summary>
	// This function generates a schedule of dates starting at the anchor date, incrementing by
	// the period days, period count times.
	// </summary>
	// <param name="anchor">The anchor date of the schedule</param>
	// <param name="period_count">The number of periods in the schedule</param>
	// <param name="period_days">The number of days in each period</param>
	// <returns>A schedule of dates</returns>
	inline
	std::vector<year_month_day>
	genDailySched(
		const year_month_day& anchor,
		unsigned int period_count,
		days period_days)
	{
		std::vector<year_month_day> sched;
		for (unsigned int period = 0; period <= period_count; ++period)
			sched.push_back(sys_days{anchor} + duration_cast<days>(period * period_days));
		return sched;
	}

	// <summary>
	// This function generates a schedule of dates startng at the anchor date, incrementing by
	// the period months, period count times. The edn of month flag indicates whether the dates
	// are intended to indicate the end of each month.
	// </summary>
	// <param name="anchor">The anchor date of the schedule</param>
	// <param name="EOMFlag">A boolean flag indicating whether intermediate dates are assumed to lie at the end of the month</param>
	// <param name="period_count">The number of periods in the schedule</param>
	// <param name="period_length">The number of months in each period</param>
	// <returns>A schedule of dates</returns>
	inline
	std::vector<year_month_day>
	genMonthlySched(
		const year_month_day& anchor,
		bool isEOM,
		unsigned int period_count,
		months period_length)
	{
		auto anchor1 = year_month_day{anchor.year(), anchor.month(), 1d};
		auto d1 = anchor.day();

		std::vector<year_month_day> sched;
		for (unsigned int period = 0; period <= period_count; ++period)
		{
			auto next = addMonths(anchor1, period * period_length, true);
			auto y = next.year();
			auto m = next.month();
			auto days_in_month = lastDayOfMonth(y, m);
			auto d = (isEOM ? days_in_month : std::min(days_in_month, d1));
			sched.push_back(y / m / d);
		}
		return sched;
	}

	// <summary>
	// This function trims a schedule by removing dates up to the first valid date. The include
	// pervious date flag indicates whter the first valid date should be included in the returned schedule.
	// </summary>
	// <param name="schedule">The schedule of dates</param>
	// <param name="firstValidDate">The first valid date in the schedule</param>
	// <param name="includePreviousDate">If true incldue the first valid date in the returned schedule</param>
	// <returns>A schedule of dates</returns>
	inline
	std::vector<year_month_day>
	ltrimSched(const std::vector<year_month_day>& Schedule, const year_month_day& FirstValidDate, bool IncludePreviousDate)
	{
		size_t i = 0;
		while (i < Schedule.size())
		{
			if (Schedule[i] > FirstValidDate)
				break;
			++i;
		}
		if (IncludePreviousDate)
		{
			if (i == 0)
				throw std::invalid_argument("There is no date prior to the supplied date. ");
			--i;
		}
		size_t count = Schedule.size() - i;
		if (count == 0)
			return std::vector<year_month_day>();
		else
		{
			std::vector<year_month_day> sched2;
			while (i < Schedule.size())
			{
				sched2.push_back(Schedule[i]);
				++i;
			}
			return sched2;
		}
	}

	inline std::vector<year_month_day> ltrim(const std::vector<year_month_day>& schedule, const year_month_day& firstValidDate)
	{
		std::vector<year_month_day> _schedule;
		for (std::vector<year_month_day>::const_iterator i_date(schedule.begin()); i_date != schedule.end(); ++i_date)
			if (*i_date >= firstValidDate)
				_schedule.push_back(*i_date);
		return _schedule;
	}

	inline std::vector<year_month_day> rtrim(const std::vector<year_month_day>& schedule, const year_month_day& lastValidDate)
	{
		std::vector<year_month_day> _schedule;
		for (std::vector<year_month_day>::const_iterator i_date(schedule.begin()); i_date != schedule.end(); ++i_date)
			if (*i_date <= lastValidDate)
				_schedule.push_back(*i_date);
		return _schedule;
	}

	inline std::vector<year_month_day> trim(const std::vector<year_month_day>& schedule, const year_month_day& firstValidDate, const year_month_day& lastValidDate)
	{
		return ltrim(rtrim(schedule, lastValidDate), firstValidDate);
	}

	// <summary>
	// This function generates a schedule starting at trade date plus settle days with a period
	// length of months and a frequency of frequency.
	// </summary>
	// <param name="tradeDate">The start of the schedule</param>
	// <param name="settleDays">The number of days to adjust the trade date by</param>
	// <param name="monthsInPeriod">The number of months in each period</param>
	// <param name="frequency">The frequency of the schedule</param>
	// <param name="EOMFlag">A boolean flag indicating whether intermediate dates are assumed to lie at the end of the month</param>
	// <param name="dateRule">The business date adjustment method</param>
	// <param name="holidays">The list of dates which are considered holidays</param>
	// <returns>A schedule of dates</returns>
	inline
	std::vector<year_month_day>
	genSched(
		const year_month_day& tradeDate, 
		days settleDays,
		months monthsInPeriod,
		EFrequency frequency,
		bool isEOM,
		EDateRule dateRule,
		const std::set<year_month_day>& hols)
	{
		if (monthsInPeriod < months{1})
			throw std::invalid_argument("Months must be greater than zero");

		auto start_date = addBusinessDays(tradeDate, settleDays, hols);

		std::vector<year_month_day> sched;
		switch (frequency)
		{
		case EFrequency::None:
			throw std::runtime_error("Unhandled frequency");
			
		case EFrequency::Daily:
			{
				auto maturity_date = adjust(addMonths(start_date, monthsInPeriod, true), dateRule, hols);
				auto periods = duration_cast<days>(sys_days{maturity_date} - sys_days{start_date});
				sched = adjustSchedule(genDailySched(start_date, periods.count(), days{1}), dateRule, hols);
				sched.erase(std::unique(sched.begin(), sched.end()), sched.end());
			}
			break;
		case EFrequency::Weekly:
			{
				auto maturity_date = adjust(addMonths(start_date, monthsInPeriod, true), dateRule, hols);
				auto period_days = duration_cast<days>(sys_days{maturity_date} - sys_days{start_date});
				auto weeks = period_days / 7;
				auto odd_days = period_days % 7;
				auto adj_start_date = sys_days{start_date} + odd_days;
				auto tmp_sched = adjustSchedule(genDailySched(adj_start_date, weeks.count(), days{7}), dateRule, hols);
				if (odd_days > days{0})
					sched.push_back(start_date);
				for (size_t i = 0; i < tmp_sched.size(); ++i)
					sched.push_back(tmp_sched[i]);
			}
			break;
		case EFrequency::Annual:
		case EFrequency::SemiAnnual:
		case EFrequency::Quarterly:
		case EFrequency::Monthly:
			{
				auto period_length = months{12 / std::to_underlying(frequency)};
				auto period_count = monthsInPeriod / period_length;
				if (monthsInPeriod % period_length != months{0})
					throw std::invalid_argument("not a whole number of periods");
				sched = adjustSchedule(genMonthlySched(start_date, isEOM, period_count, period_length), dateRule, hols);
			}
			break;
		}

		return sched;
	}

	inline std::vector<year_month_day> generateSchedule(const year_month_day& effective_date, const Tenor tenor, EDateRule date_rule, bool eom_flag, EFrequency frequency, bool odd_at_start, const std::set<year_month_day>& holidays = std::set<year_month_day>())
	{
		auto start_date = tenor.spot_date(effective_date, date_rule, holidays);

		auto end_date = start_date;
		if (tenor.days > std::chrono::days{0})
			end_date = addBusinessDays(end_date, tenor.days, holidays);
		end_date = sys_days{end_date} + tenor.weeks;
		end_date = addMonths(end_date, tenor.months, true);
		end_date = adjust(end_date, date_rule, holidays);

		return generateSchedule(start_date, end_date, eom_flag, frequency, odd_at_start);
	}
}

#endif // __jetblack__dates__tenor_schedules_hpp
