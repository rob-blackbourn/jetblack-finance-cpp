#include "rates/deposit.hpp"
#include "rates/yield_curve.hpp"

#include <chrono>

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

using namespace dates;
using namespace rates;

TEST_CASE("ctor.default", "deposit")
{
    auto deposit = Deposit{};

    REQUIRE( deposit.rate() == Approx(0.0).epsilon(1e-12) );
}

TEST_CASE("ctor.dates", "deposit")
{
    auto deposit = Deposit{1.0, 0.05, 2026y/January/5d, 2026y/March/5d, EDayCount::Actual_d365};

    REQUIRE( deposit.startDate() == 2026y/January/5d );
    REQUIRE( deposit.endDate() == 2026y/March/5d );
    REQUIRE( deposit.rate() == Approx(0.05).epsilon(1e-12) );
    REQUIRE( deposit.dayCount() == EDayCount::Actual_d365 );
}

TEST_CASE("ctor.tenor", "deposit")
{
    auto deposit = Deposit{
        1e6,
        0.05,
        2026y/January/5d,
        months{3},
        EDayCount::Actual_d365,
        EDateRule::ModFollowing,
        {2026y/January/1d, 2026y/April/5d, 2026y/May/1d, 2026y/December/25}
    };

    REQUIRE( deposit.notional() == 1e6 );
    REQUIRE( deposit.startDate() == 2026y/January/5d );
    REQUIRE( deposit.endDate() == 2026y/April/6d );
    REQUIRE( deposit.rate() == Approx(0.05).epsilon(1e-12) );
    REQUIRE( deposit.dayCount() == EDayCount::Actual_d365 );
}

TEST_CASE("value", "deposit")
{
    auto deposit = Deposit{1e6, 0.05, 2026y/January/5d, 2026y/March/5d, EDayCount::Actual_d365};
    auto curve = YieldCurve{0.05, 2026y/January/2d, EDayCount::Actual_d365};
    auto actual = deposit.value(curve);
    auto expected = -32.472115593855456;
    REQUIRE( actual == Approx(expected).epsilon(1e-12) );
}

TEST_CASE("calculateZeroRate", "deposit")
{
    auto deposit = Deposit{1e6, 0.05, 2026y/January/5d, 2026y/March/5d, EDayCount::Actual_d365};
    auto curve = YieldCurve{0.05, 2026y/January/2d, EDayCount::Actual_d365};
    auto actual = deposit.calculateZeroRate(curve);
    auto expected = 0.04980875182891082;
    REQUIRE( actual == Approx(expected).epsilon(1e-10) );
}

TEST_CASE("solveZeroRate", "deposit")
{
    auto valueDate = 2026y/January/2d;
    auto deposit = Deposit{1.0, 0.05, 2026y/January/5d, 2026y/March/5d, EDayCount::Actual_d365};
    auto t = yearFrac(valueDate, deposit.endDate(), EDayCount::Actual_d365);
    auto curve = YieldCurve{{{t, 0.05}}, valueDate, EDayCount::Actual_d365};
    auto actual = deposit.solveZeroRate(curve);
    auto expected = 0.049799027345635349;
    REQUIRE( actual == Approx(expected).epsilon(1e-10) );
}

TEST_CASE("solveZeroRate/ON", "deposit")
{
    auto valueDate = 1997y/October/6d;
    auto deposit = Deposit(1e6, 5.58675 / 100, valueDate, days{1}, EDayCount::Actual_d360, EDateRule::Following, {});
    auto t = yearFrac(valueDate, deposit.endDate(), EDayCount::Actual_d365);
    auto curve = YieldCurve{{{1, 0.05}}, valueDate, EDayCount::Actual_d365, EInterpolationMethod::Exponential};
    auto actual = deposit.solveZeroRate(curve);
    auto expected = 0.056639042777946388;
    REQUIRE( actual == Approx(expected).epsilon(1e-10) );
}
