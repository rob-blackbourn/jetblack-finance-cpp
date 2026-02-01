#include "rates/ir_swap.hpp"

#include <chrono>

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

using namespace dates;
using namespace rates;

TEST_CASE("ctor.default", "ir_swap")
{
    auto swap = IrSwap();

    REQUIRE ( swap.fixedLeg().rate() == 0.0 );
    REQUIRE ( swap.floatingLeg().spread() == 0.0 );
}

TEST_CASE("ctor.tenor", "[ir_swap]")
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
    auto fixLag = days{2};
    double spread = 0.01;

    auto swap = IrSwap(
        notional,
        rate,
        spread,
        startDate,
        tenor,
        frequency,
        stubType,
        dateRule,
        dayCount,
        fixLag,
        holidays
    );

    REQUIRE ( swap.fixedLeg().rate() == 0.05 );
    REQUIRE ( swap.fixedLeg().startDate() == 2000y/January/1d );
    REQUIRE ( swap.fixedLeg().maturity() == 2002y/January/1d );

    REQUIRE ( swap.floatingLeg().spread() == 0.01 );
    REQUIRE ( swap.floatingLeg().startDate() == 2000y/January/1d );
    REQUIRE ( swap.floatingLeg().maturity() == 2002y/January/1d );
}
