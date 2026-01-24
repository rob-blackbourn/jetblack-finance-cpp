#include "dates/arithmetic.hpp"

#include <chrono>

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

using namespace dates;

TEST_CASE("addMonths", "[dates]")
{
    using namespace std::chrono;

    auto aug31 = 2000y/August/31d;

    auto sep30 = addMonths(aug31, months{1}, true);
    REQUIRE ( sep30 == 2000y/September/30d);

    auto jul31 = addMonths(aug31, months{-1}, true);
    REQUIRE ( jul31 == 2000y/July/31d);

    auto feb29 = 2000y/February/29d;

    auto mar31 = addMonths(feb29, months{1}, true);
    REQUIRE ( mar31 == 2000y/March/31d);

    auto mar29 = addMonths(feb29, months{1}, false);
    REQUIRE ( mar29 == 2000y/March/29d);

    auto jan31 = addMonths(feb29, months{-1}, true);
    REQUIRE ( jan31 == 2000y/January/31d);

    auto jan29 = addMonths(feb29, months{-1}, false);
    REQUIRE ( jan29 == 2000y/January/29d);
}

TEST_CASE("dayOfWeek", "[dates]")
{
    using namespace std::chrono;

    //    December 2014
    // Su Mo Tu We Th Fr Sa
    //     1  2  3  4  5  6
    //  7  8  9 10 11 12 13
    // 14 15 16 17 18 19 20
    // 21 22 23 24 25 26 27
    // 28 29 30 31

    auto mon = dayOfWeek(2014y/December/1d);
    REQUIRE( mon == Monday ); 

    auto fri = dayOfWeek(2014y/December/5d);
    REQUIRE( fri == Friday ); 

    auto sat = dayOfWeek(2014y/December/20d);
    REQUIRE( sat == Saturday ); 

    auto sun = dayOfWeek(2014y/December/28d);
    REQUIRE( sun == Sunday ); 
}

TEST_CASE("lastDayOfMonth", "[dates]")
{
    using namespace std::chrono;

    REQUIRE (lastDayOfMonth(2009y, January) == 31d); // 28 days in January.
    REQUIRE( lastDayOfMonth(2009y, February) == 28d); // 28 days in February in a non leap year
    REQUIRE( lastDayOfMonth(2009y, March) == 31d); // There are 31 days in March.
    REQUIRE( lastDayOfMonth(2009y, April) == 30d); // There are 30 days in April.
    REQUIRE( lastDayOfMonth(2009y, May) == 31d); // There are 31 days in May.
    REQUIRE( lastDayOfMonth(2009y, June) == 30d); // There are 30 days in June.
    REQUIRE( lastDayOfMonth(2009y, July) == 31d); // There are 31 days in July.
    REQUIRE( lastDayOfMonth(2009y, August) == 31d); // There are 31 days in August.
    REQUIRE( lastDayOfMonth(2009y, September) == 30d); // There are 30 days in September.
    REQUIRE( lastDayOfMonth(2009y, October) == 31d); // There are 31 days in October.
    REQUIRE( lastDayOfMonth(2009y, November) == 30d); // There are 30 days in November.
    REQUIRE( lastDayOfMonth(2009y, December) == 31d); // There are 31 days in December.
    REQUIRE( lastDayOfMonth(2008y, February) == 29d); // There are 29 days in February in a leap year.
}

TEST_CASE("dayOfYear", "[dates]")
{
    REQUIRE( days{193} == dayOfYear(2026y/July/12d) );
}

TEST_CASE("addNthDayOfWeek", "[dates]")
{
    auto startDate = 2026y/January/1d;
    auto resultDate = addNthDayOfWeek(startDate, 3, Wednesday, true);
    REQUIRE( resultDate == 2026y/January/21d );
}
