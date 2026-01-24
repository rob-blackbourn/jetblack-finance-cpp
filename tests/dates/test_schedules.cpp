#include "dates/schedules.hpp"

#include <algorithm>
#include <chrono>
#include <ranges>
#include <vector>

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

using namespace dates;

TEST_CASE("schedule backwards", "[dates]")
{
    auto schedule = generateSchedule(
        2000y/January/1d,
        2002y/January/1d,
        EFrequency::SemiAnnual,
        EStubType::ShortFirst,
        EDateRule::ModFollowing,
        {}
    );

    auto expected = std::vector<year_month_day> {
        2000y/January/3d,
        2000y/July/3d,
        2001y/January/1d,
        2001y/July/2d,
        2002y/January/1d,
    };

    REQUIRE ( schedule == expected );
}

TEST_CASE("schedule - short first", "[dates]")
{
    auto schedule = generateSchedule(
        2000y/March/1d,
        2002y/January/1d,
        EFrequency::SemiAnnual,
        EStubType::ShortFirst,
        EDateRule::ModFollowing,
        {}
    );

    auto expected = std::vector<year_month_day> {
        2000y/March/1d,
        2000y/July/3d,
        2001y/January/1d,
        2001y/July/2d,
        2002y/January/1d,
    };

    REQUIRE ( schedule == expected );
}

TEST_CASE("schedule - long first", "[dates]")
{
    auto schedule = generateSchedule(
        2000y/March/1d,
        2002y/January/1d,
        EFrequency::SemiAnnual,
        EStubType::LongFirst,
        EDateRule::ModFollowing,
        {}
    );

    auto expected = std::vector<year_month_day> {
        2000y/March/1d,
        2001y/January/1d,
        2001y/July/2d,
        2002y/January/1d,
    };

    REQUIRE ( schedule == expected );
}

TEST_CASE("schedule forward", "[dates]")
{
    auto schedule = generateSchedule(
        2000y/January/1d,
        2002y/January/1d,
        EFrequency::SemiAnnual,
        EStubType::ShortLast,
        EDateRule::ModFollowing,
        {}
    );

    auto expected = std::vector<year_month_day> {
        2000y/January/3d,
        2000y/July/3d,
        2001y/January/1d,
        2001y/July/2d,
        2002y/January/1d,
    };

    REQUIRE ( schedule == expected );
}

TEST_CASE("schedule short last", "[dates]")
{
    auto schedule = generateSchedule(
        2000y/January/1d,
        2001y/October/1d,
        EFrequency::SemiAnnual,
        EStubType::ShortLast,
        EDateRule::ModFollowing,
        {}
    );

    auto expected = std::vector<year_month_day> {
        2000y/January/3d,
        2000y/July/3d,
        2001y/January/1d,
        2001y/July/2d,
        2001y/October/1d,
    };

    REQUIRE ( schedule == expected );
}

TEST_CASE("schedule long last", "[dates]")
{
    auto schedule = generateSchedule(
        2000y/January/1d,
        2001y/October/1d,
        EFrequency::SemiAnnual,
        EStubType::LongLast,
        EDateRule::ModFollowing,
        {}
    );

    auto expected = std::vector<year_month_day> {
        2000y/January/3d,
        2000y/July/3d,
        2001y/January/1d,
        2001y/October/1d,
    };

    REQUIRE ( schedule == expected );
}
