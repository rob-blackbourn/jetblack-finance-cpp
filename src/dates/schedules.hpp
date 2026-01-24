#ifndef __jetblack__dates__schedules_hpp
#define __jetblack__dates__schedules_hpp

#include <chrono>
#include <stdexcept>
#include <variant>

#include "dates/arithmetic.hpp"
#include "dates/time_units.hpp"

namespace dates
{
	using namespace std::chrono;
	using namespace stdext;
	
	enum class EStubType
	{
		ShortFirst,
		LongFirst,
		ShortLast,
		LongLast,
	};
	enum class EFrequency
	{
		None = 0,
		Annual = 1,
		SemiAnnual = 2,
		Quarterly = 4,
		Monthly = 12,
		Weekly = 52,
		Daily = 365
	};

	inline time_unit_t to_time_unit(EFrequency frequency)
	{
		switch (frequency)
		{
		case EFrequency::Annual:
			return years(1);
		case EFrequency::SemiAnnual:
			return months(6);
		case EFrequency::Quarterly:
			return months(3);
		case EFrequency::Monthly:
			return months(1);
		case EFrequency::Weekly:
			return weeks(1);
		case EFrequency::Daily:
			return days(1);
		default:
			throw std::runtime_error("unknown frequency for TimePeriod");
		}
	}

	inline std::vector<year_month_day> generateScheduleBackwards(
		const year_month_day& startDate,
		const year_month_day& endDate,
		EFrequency frequency,
		bool allowShortFirst,
		EDateRule dateRule,
		const std::set<year_month_day>& holidays)
	{
		// Generate the schedule from the end date, going backwards
		auto startDateAdj = adjust(startDate, dateRule, holidays);
		bool eom = isEndOfMonth(endDate);
		auto step = to_time_unit(frequency);

		auto schedule = std::vector<year_month_day>{};

		int n = 0;
		auto date = adjust(endDate, dateRule, holidays);
		while (date >= startDateAdj)
		{
			// The dates are added to the end. The schedule will be
			// reversed just before it is returned.
			schedule.push_back(date);
			date = add(endDate, step * --n, eom, dateRule, holidays);
		}

		if (!schedule.empty() && schedule.back() > startDateAdj)
		{
			// The step was too big for the start date to be added.
			// There are two choices:
			// 1. Allow a short first period.
			// 2. Create a long first period. If the schedule only contains
			//.   one date a, short period is the only solution.
			if (allowShortFirst || schedule.size() == 1)
				schedule.push_back(startDateAdj);
			else
				schedule.back() = startDateAdj;
		}

		// Put it into the right order.
		std::ranges::reverse(schedule);

		return schedule;
	}

	inline std::vector<year_month_day> generateScheduleForwards(
		const year_month_day& startDate,
		const year_month_day& endDate,
		EFrequency frequency,
		bool allowShortLast,
		EDateRule dateRule,
		const std::set<year_month_day>& holidays)
	{
		// Generate the schedule from the start date, going forwards.
		auto endDateAdj = adjust(endDate, dateRule, holidays);
		bool eom = isEndOfMonth(startDate);
		auto step = to_time_unit(frequency);

		auto schedule = std::vector<year_month_day>{};

		int n = 0;
		auto date = adjust(startDate, dateRule, holidays);
		while (date <= endDateAdj)
		{
			schedule.push_back(date);
			date = add(startDate, step * ++n, eom, dateRule, holidays);
		}

		if (!schedule.empty() && schedule.back() < endDateAdj)
		{
			// The step was too big for the end date to be added.
			// There are two choices:
			// 1. Allow a short last period.
			// 2. Create a long last period. If the schedule only contains
			//.   one date a, short period is the only solution.
			if (allowShortLast || schedule.size() == 1)
				schedule.push_back(endDateAdj);
			else
				schedule.back() = endDateAdj;
		}

		return schedule;
	}

	inline std::vector<std::chrono::year_month_day> generateSchedule(
		const std::chrono::year_month_day& startDate,
		const std::chrono::year_month_day& endDate,
		EFrequency frequency,
		EStubType stubType,
		EDateRule dateRule,
		const std::set<std::chrono::year_month_day>& holidays)
	{
		if (startDate >= endDate)
			throw "start date must be prior to end date";

		switch (stubType)
		{
			case EStubType::ShortFirst:
			case EStubType::LongFirst:
				
				return generateScheduleBackwards(
					startDate,
					endDate,
					frequency,
					stubType == EStubType::ShortFirst,
					dateRule,
					holidays);

			case EStubType::ShortLast:
			case EStubType::LongLast:

				return generateScheduleForwards(
					startDate,
					endDate,
					frequency,
					stubType == EStubType::ShortLast,
					dateRule, holidays);

			default:

				throw std::runtime_error("Unknown stub type");
		}
	}
}


static const struct { const char* string_type; dates::EStubType enum_type; } StubType_TypeMap[] =
{
	{"SHORTFIRST",	dates::EStubType::ShortFirst},
	{"SF",			dates::EStubType::ShortFirst},
	{"SHORT_FIRST",	dates::EStubType::ShortFirst},

	{"SHORTLAST",	dates::EStubType::ShortLast},
	{"SL",			dates::EStubType::ShortLast},
	{"SHORT_LAST",	dates::EStubType::ShortLast},

	{"LONGFIRST",	dates::EStubType::LongFirst},
	{"LF",			dates::EStubType::LongFirst},
	{"LONG_FIRST",	dates::EStubType::LongFirst},

	{"LONGLAST",	dates::EStubType::LongLast},
	{"LL",			dates::EStubType::LongLast},
	{"LONG_LAST",	dates::EStubType::LongLast}
};

inline dates::EStubType& operator>>(const std::string& lhs, dates::EStubType& rhs)
{
	for (size_t i = 0; i < sizeof(StubType_TypeMap) / sizeof(StubType_TypeMap[0]); ++i)
		if (std::strcmp(lhs.c_str(), StubType_TypeMap[i].string_type) == 0)
			return rhs = StubType_TypeMap[i].enum_type;

	throw std::invalid_argument("unknown enum");
}

inline std::string& operator<<(std::string& lhs, const dates::EStubType& rhs)
{
	for (size_t i = 0; i < sizeof(StubType_TypeMap) / sizeof(StubType_TypeMap[0]); ++i)
		if (rhs == StubType_TypeMap[i].enum_type)
			return lhs = StubType_TypeMap[i].string_type;

	throw std::invalid_argument("unknown enum");
}

inline std::ostream& operator<<(std::ostream& lhs, const dates::EStubType& rhs)
{
	std::string s;
	s << rhs;
	lhs << s;
	return lhs;
}

static const struct { const char* string_type; dates::EFrequency enum_type; } Frequency_TypeMap[] =
{
	{ "ANNUAL",			dates::EFrequency::Annual},
	{ "A",				dates::EFrequency::Annual},
	{ "SEMI ANNUAL",	dates::EFrequency::SemiAnnual},
	{ "SA",				dates::EFrequency::SemiAnnual},
	{ "QUARTERLY",		dates::EFrequency::Quarterly},
	{ "Q",				dates::EFrequency::Quarterly},
	{ "MONTHLY",		dates::EFrequency::Monthly},
	{ "M",				dates::EFrequency::Monthly},
	{ "WEEKLY",			dates::EFrequency::Weekly},
	{ "W",				dates::EFrequency::Weekly},
	{ "DAILY",			dates::EFrequency::Daily},
	{ "D",				dates::EFrequency::Daily}
};

inline dates::EFrequency& operator>>(const std::string& lhs, dates::EFrequency& rhs)
{
	for (size_t i = 0; i < sizeof(Frequency_TypeMap) / sizeof(Frequency_TypeMap[0]); ++i)
		if (std::strcmp(lhs.c_str(), Frequency_TypeMap[i].string_type) == 0)
			return rhs = Frequency_TypeMap[i].enum_type;

	throw std::invalid_argument("unknown enum");
}

inline std::string& operator<<(std::string& lhs, const dates::EFrequency& rhs)
{
	for (size_t i = 0; i < sizeof(Frequency_TypeMap) / sizeof(Frequency_TypeMap[0]); ++i)
		if (rhs == Frequency_TypeMap[i].enum_type)
			return lhs = Frequency_TypeMap[i].string_type;

	throw std::invalid_argument("unknown enum");
}

inline std::ostream& operator<<(std::ostream& lhs, const dates::EFrequency& rhs)
{
	std::string s;
	s << rhs;
	lhs << s;
	return lhs;
}

#endif // __jetblack__dates__schedules_hpp
