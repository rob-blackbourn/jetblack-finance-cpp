#include "rates/ir_future.hpp"
#include "rates/yield_curve.hpp"

#include <chrono>

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

using namespace dates;
using namespace rates;

TEST_CASE("ctor.default", "[ir_future]")
{
    auto future = IrFuture{};

    REQUIRE( fabs(future.rate() - 0.0) < 1e-12 );
}

TEST_CASE("ctor.dates", "[ir_future]")
{
    auto future = IrFuture{
        2026y/March/18d,
        EDayCount::Actual_d365,
        95.0,
        0.0,
        EDateRule::Following,
        {2026y/January/1d, 2026y/April/5d, 2026y/May/1d, 2026y/December/25},
        days{2}
    };

    REQUIRE( future.startDate() == 2026y/March/20d );
    REQUIRE( future.endDate() == 2026y/June/22d );
    REQUIRE( fabs(future.rate() - 0.05) < 1e-12 );
    REQUIRE( future.dayCount() == EDayCount::Actual_d365 );
}


TEST_CASE("value", "[ir_future]")
{
    auto future = IrFuture{
        2026y/March/18d,
        EDayCount::Actual_d365,
        95.0,
        0.0,
        EDateRule::Following,
        {2026y/January/1d, 2026y/April/5d, 2026y/May/1d, 2026y/December/25},
        days{2}
    };

    auto yield_curve = YieldCurve{"flat", 0.05, 2026y/January/2d, EDayCount::Actual_d365};
    auto actual = future.value(yield_curve);
    auto expected = -0.0000813341;
    REQUIRE( fabs(actual - expected) < 1e-10 );
}

TEST_CASE("calculateZeroRate", "[ir_future]")
{
    auto future = IrFuture{
        2026y/March/18d,
        EDayCount::Actual_d365,
        95.0,
        0.0,
        EDateRule::Following,
        {2026y/January/1d, 2026y/April/5d, 2026y/May/1d, 2026y/December/25},
        days{2}
    };

    auto yield_curve = YieldCurve{"flat", 0.05, 2026y/January/2d, EDayCount::Actual_d365};
    auto actual = future.calculateZeroRate(yield_curve);
    auto expected = 0.0498245439;
    // REQUIRE( actual == expected);
    REQUIRE( fabs(actual - expected) < 1e-10 );
}

TEST_CASE("solveZeroRate", "[ir_future]")
{
    auto future = IrFuture{
        2026y/March/18d,
        EDayCount::Actual_d365,
        95.0,
        0.0,
        EDateRule::Following,
        {2026y/January/1d, 2026y/April/5d, 2026y/May/1d, 2026y/December/25},
        days{2}
    };
    auto yield_curve = YieldCurve{"flat", 0.05, 2026y/January/2d, EDayCount::Actual_d365};
    auto actual = future.solveZeroRate(yield_curve, 0);
    auto expected = 0.0496808193;
    // REQUIRE(actual == expected);
    REQUIRE( fabs(actual - expected) < 1e-10 );
}
