#include "dates/arithmetic.hpp"
#include "dates/business_days.hpp"

#include <chrono>

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

using namespace dates;

TEST_CASE("isWeekend", "[dates]")
{
    using namespace std::chrono;

    // 19 December 2014 was a Friday.
    REQUIRE_FALSE (isWeekend(year_month_day{2014y/December/19d}));

    // 20 December 2014 was a Saturday.
    REQUIRE (isWeekend(year_month_day{2014y/December/20d}));

    // 21 December 2014 was a Sunday.
    REQUIRE (isWeekend(year_month_day{2014y/December/21d}));

    // 22 December 2014 was a Monday.
    REQUIRE_FALSE (isWeekend(year_month_day{2014y/December/22d}));
}

TEST_CASE("isHoliday", "[dates]")
{
    using namespace std::chrono;

    //    December 2014
    // Su Mo Tu We Th Fr Sa
    //     1  2  3  4  5  6
    //  7  8  9 10 11 12 13
    // 14 15 16 17 18 19 20
    // 21 22 23 24 25 26 27
    // 28 29 30 31

    auto holidays = std::set<year_month_day> {
        year_month_day{2014y/December/25d},
        year_month_day{2014y/December/26d},
    };

    REQUIRE_FALSE (isHoliday(year_month_day{2014y/December/24d}, holidays));
    REQUIRE (isHoliday(year_month_day{2014y/December/25d}, holidays));
    REQUIRE (isHoliday(year_month_day{2014y/December/26d}, holidays));
    REQUIRE_FALSE (isHoliday(year_month_day{2014y/December/27d}, holidays));
}

TEST_CASE("isBusinessDay", "[dates]")
{
    using namespace std::chrono;

    //    December 2014
    // Su Mo Tu We Th Fr Sa
    //     1  2  3  4  5  6
    //  7  8  9 10 11 12 13
    // 14 15 16 17 18 19 20
    // 21 22 23 24 25 26 27
    // 28 29 30 31

    auto holidays = std::set<year_month_day> {
        year_month_day{2014y/December/25d},
        year_month_day{2014y/December/26d},
    };

    REQUIRE (isBusinessDay(year_month_day{2014y/December/24d}, holidays));
    REQUIRE_FALSE (isBusinessDay(year_month_day{2014y/December/25d}, holidays));
    REQUIRE_FALSE (isBusinessDay(year_month_day{2014y/December/26d}, holidays));
    REQUIRE_FALSE (isBusinessDay(year_month_day{2014y/December/27d}, holidays));
    REQUIRE_FALSE (isBusinessDay(year_month_day{2014y/December/28d}, holidays));
    REQUIRE (isBusinessDay(year_month_day{2014y/December/29d}, holidays));
}

TEST_CASE("addBusinessDays", "[dates]")
{
    using namespace std::chrono;

    auto holidays = std::set<year_month_day> {
        2015y/January/1d,
        2015y/April/3d,
        2015y/April/6d,
        2015y/May/1d,
        2015y/December/25d,
        2015y/December/16d,
    };

    // Forward
    // - Should skip New Years Day.
    REQUIRE( 2015y/1/8d == addBusinessDays(2015y/1/1d, days{5}, holidays) );
    // - Nothing to skip. 
    REQUIRE( 2015y/1/8d == addBusinessDays(2015y/1/2d, days{4}, holidays) ); 
    // - Nothing to skip.
    REQUIRE( 2015y/1/2d == addBusinessDays(2014y/12/29d, days{3}, holidays) ); 
    // Back
    // - Should skip New Years Day.
    REQUIRE( 2014y/12/31d == addBusinessDays(2015y/1/8d, days{-5}, holidays) );
    // - Nothing to skip.
    REQUIRE( 2015y/1/2d == addBusinessDays(2015y/1/8d, days{-4}, holidays) ); 
    // - Nothing to skip.
    REQUIRE( 2014y/12/29d == addBusinessDays(2015y/1/2d, days{-3}, holidays) ); 
}

TEST_CASE("nearestBusinessDay", "[dates]")
{
    using namespace std::chrono;

    //            July 2015
    // Su Mo Tu We Th Fr Sa
    //           1  2  3  4
    //  5  6  7  8  9 10 11
    // 12 13 14 15 16 17 18
    // 19 20 21 22 23 24 25
    // 26 27 28 29 30 31
    auto holidays = std::set<year_month_day> {
        2015y/July/13d
    };

    // Saturday should roll to Friday
    REQUIRE( 2015y/July/3d == nearestBusinessDay(2015y/July/4d, true, holidays) );
    // Sunday should roll to Monday
    REQUIRE( 2015y/July/6d == nearestBusinessDay(2015y/July/5d, true, holidays)); 
    // Sunday should prefer to roll to Tuesday
    REQUIRE( 2015y/July/14d == nearestBusinessDay(2015y/July/12d, true, holidays)); 
    // Sunday should prefer to roll to Friday
    REQUIRE( 2015y/July/10d == nearestBusinessDay(2015y/July/12d, false, holidays)); 
}
