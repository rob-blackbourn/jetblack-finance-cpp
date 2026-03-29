#include "dates/tenor.hpp"

#include <chrono>

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

using namespace std::chrono;
using namespace dates;

TEST_CASE("Tenor.ctor", "[dates]")
{
    auto t1 = Tenor(EFrequency::Quarterly);
    REQUIRE (t1.spot_days == days{0});
    REQUIRE (t1.days == days{0});
    REQUIRE (t1.weeks == weeks{0});
    REQUIRE (t1.months == months{3});

    auto t2 = Tenor("3M");
    REQUIRE (t2.spot_days == days{0});
    REQUIRE (t2.days == days{0});
    REQUIRE (t2.weeks == weeks{0});
    REQUIRE (t2.months == months{3});
}
