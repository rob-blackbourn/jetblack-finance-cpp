#include "rates/ir_swap.hpp"
#include "rates/yield_curve.hpp"

#include <chrono>

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

using namespace dates;
using namespace rates;

TEST_CASE("ctor.default", "[ir_swap_leg_fixed]")
{
    auto leg = IrSwapLegFixed();

    REQUIRE ( leg.rate() == 0.0 );
}

TEST_CASE("ctor.tenor", "[ir_swap_leg_fixed]")
{
    auto startDate = 2000y/January/1d;
    auto tenor = years(2);
    auto frequency = EFrequency::SemiAnnual;
    auto stubType = EStubType::ShortFirst;
    auto dateRule = EDateRule::ModFollowing;
    auto holidays = std::set<year_month_day> {};
    auto dayCount = EDayCount::Actual_d365;
    double notional = 1e6;
    double rate = 0.05;

    auto leg = IrSwapLegFixed(
        notional,
        rate,
        startDate,
        tenor,
        frequency,
        stubType,
        dateRule,
        dayCount,
        holidays
    );

    REQUIRE ( leg.rate() == 0.05 );
    REQUIRE ( leg.startDate() == 2000y/January/1d );
    REQUIRE ( leg.endDate() == 2002y/January/1d );
}
