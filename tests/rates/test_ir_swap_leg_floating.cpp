#include "rates/ir_swap.hpp"
#include "rates/yield_curve.hpp"

#include <chrono>

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

using namespace dates;
using namespace rates;

TEST_CASE("ctor.default", "[ir_swap_leg_floating]")
{
    auto leg = IrSwapLegFloating();

    REQUIRE ( leg.spread() == 0.0 );
}

TEST_CASE("ctor.tenor", "[ir_swap_leg_floating]")
{
    auto firstAccrualDate = 2000y/January/1d;
    auto tenor = years(2);
    auto frequency = EFrequency::SemiAnnual;
    auto stubType = EStubType::ShortFirst;
    auto dateRule = EDateRule::ModFollowing;
    auto holidays = std::set<year_month_day> {};
    auto dayCount = EDayCount::Actual_d365;
    double notional = 1e6;
    double spread = 0.01;

    auto leg = IrSwapLegFloating(
        notional,
        spread,
        firstAccrualDate,
        tenor,
        frequency,
        stubType,
        dateRule,
        dayCount,
        days{2},
        holidays
    );

    REQUIRE ( leg.spread() == 0.01 );
    REQUIRE ( leg.firstAccrualDate() == 2000y/January/1d );
    REQUIRE ( leg.maturityDate() == 2002y/January/1d );
}
