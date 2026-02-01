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
        1.0,
        95.0,
        2026y/March,
        EDayCount::Actual_d365,
        EDateRule::Following,
        days{2},
        {2026y/January/1d, 2026y/April/5d, 2026y/May/1d, 2026y/December/25}
    };

    REQUIRE( future.firstAccrualDate() == 2026y/March/20d );
    REQUIRE( future.maturityDate() == 2026y/June/22d );
    REQUIRE( future.rate() == Approx(0.05).epsilon(1e-12) );
    REQUIRE( future.dayCount() == EDayCount::Actual_d365 );
}


TEST_CASE("value", "[ir_future]")
{
    auto future = IrFuture{
        1.0,
        95.0,
        2026y/March,
        EDayCount::Actual_d365,
        EDateRule::Following,
        days{2},
        {2026y/January/1d, 2026y/April/5d, 2026y/May/1d, 2026y/December/25}
    };

    auto curve = YieldCurve{0.05, 2026y/January/2d, EDayCount::Actual_d365};
    auto actual = future.value(curve);
    auto expected = -0.000081334141743003596;
    REQUIRE( actual == Approx(expected).epsilon(1e-10) );
}

TEST_CASE("calculateZeroRate", "[ir_future]")
{
    auto future = IrFuture{
        1.0,
        95.0,
        2026y/March,
        EDayCount::Actual_d365,
        EDateRule::Following,
        days{2},
        {2026y/January/1d, 2026y/April/5d, 2026y/May/1d, 2026y/December/25}
    };

    auto curve = YieldCurve{0.05, 2026y/January/2d, EDayCount::Actual_d365};
    auto actual = future.calculateZeroRate(curve);
    auto expected = 0.049824543926518167;
    REQUIRE( actual == Approx(expected).epsilon(1e-10) );
}

TEST_CASE("solveZeroRate", "[ir_future]")
{
    auto future = IrFuture{
        1.0,
        95.0,
        2026y/March,
        EDayCount::Actual_d365,
        EDateRule::Following,
        days{2},
        {2026y/January/1d, 2026y/April/5d, 2026y/May/1d, 2026y/December/25}
    };
    auto curve = YieldCurve{0.05, 2026y/January/2d, EDayCount::Actual_d365};
    auto actual = future.solveZeroRate(curve);
    auto expected = 0.049680819270580927;
    REQUIRE( actual == Approx(expected).epsilon(1e-10) );
}
