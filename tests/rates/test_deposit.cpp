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

    REQUIRE( fabs(deposit.rate() - 0.0) < 1e-12 );
}

TEST_CASE("ctor.dates", "deposit")
{
    auto deposit = Deposit{2026y/January/5d, 2026y/March/5d, EDayCount::Actual_d365, 0.05};

    REQUIRE( deposit.startDate() == 2026y/January/5d );
    REQUIRE( deposit.endDate() == 2026y/March/5d );
    REQUIRE( fabs(deposit.rate() - 0.05) < 1e-12 );
    REQUIRE( deposit.dayCount() == EDayCount::Actual_d365 );
}

TEST_CASE("ctor.tenor", "deposit")
{
    auto deposit = Deposit{
        2026y/January/5d,
        months{3},
        EDayCount::Actual_d365,
        0.05,
        EDateRule::ModFollowing,
        {2026y/January/1d, 2026y/April/5d, 2026y/May/1d, 2026y/December/25}
    };

    REQUIRE( deposit.startDate() == 2026y/January/5d );
    REQUIRE( deposit.endDate() == 2026y/April/6d );
    REQUIRE( fabs(deposit.rate() - 0.05) < 1e-12 );
    REQUIRE( deposit.dayCount() == EDayCount::Actual_d365 );
}

TEST_CASE("value", "deposit")
{
    auto deposit = Deposit{2026y/January/5d, 2026y/March/5d, EDayCount::Actual_d365, 0.05};
    auto yield_curve = YieldCurve{"flat", 0.05, 2026y/January/2d, EDayCount::Actual_d365};
    auto actual = deposit.value(yield_curve);
    auto expected = -0.0000324721;
    REQUIRE( fabs(actual - expected) < 1e-10 );
}

TEST_CASE("calculateZeroRate", "deposit")
{
    auto deposit = Deposit{2026y/January/5d, 2026y/March/5d, EDayCount::Actual_d365, 0.05};
    auto yield_curve = YieldCurve{"flat", 0.05, 2026y/January/2d, EDayCount::Actual_d365};
    auto actual = deposit.calculateZeroRate(yield_curve);
    auto expected = 0.0498087518;
    REQUIRE( fabs(actual - expected) < 1e-10 );
}

TEST_CASE("solveZeroRate", "deposit")
{
    auto deposit = Deposit{2026y/January/5d, 2026y/March/5d, EDayCount::Actual_d365, 0.05};
    auto yield_curve = YieldCurve{"flat", 0.05, 2026y/January/2d, EDayCount::Actual_d365};
    auto actual = deposit.solveZeroRate(yield_curve, 0);
    auto expected = 0.0497990273;
    // REQUIRE(actual == expected);
    REQUIRE( fabs(actual - expected) < 1e-10 );
}

TEST_CASE("solveZeroRate/ON", "deposit")
{
    auto valueDate = 1997y/October/6d;
    auto deposit = Deposit(valueDate, days{1}, EDayCount::Actual_d360, 5.58675 / 100, EDateRule::Following, {});
    auto t = yearFrac(deposit.startDate(), deposit.endDate(), EDayCount::Actual_d365);
    auto yield_curve = YieldCurve{"flat", {{t, 0.05}}, valueDate, EDayCount::Actual_d365, EInterpolationMethod::Exponential};
    auto actual = deposit.solveZeroRate(yield_curve, 0);
    auto expected = 0.0566390428;
    // REQUIRE(actual == expected);
    REQUIRE( fabs(actual - expected) < 1e-10 );
}
