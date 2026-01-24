#include "dates/arithmetic.hpp"
#include "dates/business_days.hpp"
#include "dates/calendars/target.hpp"

#include <chrono>

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

using namespace std::chrono;
using namespace dates;

TEST_CASE("target", "[dates]")
{
    auto holidays = calendars::targetHolidays(year{1997}, year{2027});

    REQUIRE_FALSE (isHoliday(2014y/December/24d, holidays));
    REQUIRE (isHoliday(2014y/December/25d, holidays));
    REQUIRE (isHoliday(2014y/December/26d, holidays));
    REQUIRE_FALSE (isHoliday(2014y/December/27d, holidays));
}
