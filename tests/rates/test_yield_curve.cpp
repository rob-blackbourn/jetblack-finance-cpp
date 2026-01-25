#include "rates/yield_curve.hpp"

#include "dates/calendars/target.hpp"

#include <chrono>

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

using namespace dates;
using namespace rates;

TEST_CASE("ctor.default", "[yield_curve]")
{
    auto yc = YieldCurve{};

    REQUIRE (yc.points().empty() );
}

TEST_CASE("ctor.points", "[yield_curve]")
{
    auto yc = YieldCurve{
        "yc",
        { {0.1, 0.05}, {0.5, 0.06}, {1.0, 0.062} },
        2026y / January / 9d,
        EDayCount::Actual_d365,
        EInterpolationMethod::Hermite
    };

    REQUIRE (yc.points().size() == 3);
}

TEST_CASE("ctor.flatRate", "[yield_curve]")
{
    auto yc = YieldCurve{
        "yc",
        0.05,
        2026y / January / 9d,
        EDayCount::Actual_d365
    };

    REQUIRE (yc.points().size() == 1);
    REQUIRE (yc.points().at(0) == YieldCurvePoint{1.0, 0.05});
}

/*
ype	Settlement date	Rate (%)
Cash	Overnight rate	5.58675
Cash	Tomorrow next rate	5.59375
Cash	1m	5.625
Cash	3m	5.71875
Future	Dec-97	5.76
Future	Mar-98	5.77
Future	Jun-98	5.82
Future	Sep-98	5.88
Future	Dec-98	6.00
Swap	2y	6.01253
Swap	3y	6.10823
Swap	4y	6.16
Swap	5y	6.22
Swap	7y	6.32
Swap	10y	6.42
Swap	15y	6.56
Swap	20y	6.56
Swap	30y	6.56
*/
TEST_CASE("bootstrap", "[yield_curve]")
{
    auto valueDate = 1997y/October/6d;
    auto daysToSpot = days{2};
    auto holidays = calendars::targetHolidays(year{1997}, year{1997} + years{20});

    auto spotDate = addBusinessDays(valueDate, days{2}, holidays);

    auto depositON = Deposit(valueDate, days{1}, EDayCount::Actual_d360, 5.58675 / 100, EDateRule::Following, holidays);
    auto depositTN = Deposit(depositON.endDate(), days{1}, EDayCount::Actual_d360, 5.59375 / 100, EDateRule::Following, holidays);
    auto deposit1M = Deposit(spotDate, months{1}, EDayCount::Actual_d360, 5.625 / 100, EDateRule::Following, holidays);
    auto deposit3M = Deposit(spotDate, months{3}, EDayCount::Actual_d360, 5.71875 / 100, EDateRule::Following, holidays);
    auto deposits = std::vector<Deposit> { depositON, depositTN, deposit1M, deposit3M};

    auto futureDec97 = IrFuture(immDate(1997y/December), EDayCount::Actual_d365, 100 - 5.76, 0, EDateRule::Following, holidays, daysToSpot);
    auto futureMar98 = IrFuture(immDate(1998y/March), EDayCount::Actual_d365, 100 - 5.77, 0, EDateRule::Following, holidays, daysToSpot);
    auto futureJun98 = IrFuture(immDate(1998y/June), EDayCount::Actual_d365, 100 - 5.82, 0, EDateRule::Following, holidays, daysToSpot);
    auto futureSep98 = IrFuture(immDate(1998y/September), EDayCount::Actual_d365, 100 - 5.88, 0, EDateRule::Following, holidays, daysToSpot);
    auto futureDec98 = IrFuture(immDate(1998y/December), EDayCount::Actual_d365, 100 - 6.00, 0, EDateRule::Following, holidays, daysToSpot);
    auto futures = std::vector<IrFuture> { futureDec97, futureMar98, futureJun98, futureSep98, futureDec98};

    auto swap2Y = IrSwap(spotDate, years{2}, EFrequency::SemiAnnual, EStubType::ShortFirst, EDateRule::ModFollowing, holidays, EDayCount::Actual_d365, 1e6, 6.01253 / 100, days{2}, 0.0);
    auto swap3Y = IrSwap(spotDate, years{3}, EFrequency::SemiAnnual, EStubType::ShortFirst, EDateRule::ModFollowing, holidays, EDayCount::Actual_d365, 1e6, 6.10823 / 100, days{2}, 0.0);
    auto swap4Y = IrSwap(spotDate, years{4}, EFrequency::SemiAnnual, EStubType::ShortFirst, EDateRule::ModFollowing, holidays, EDayCount::Actual_d365, 1e6, 6.16 / 100, days{2}, 0.0);
    auto swap5Y = IrSwap(spotDate, years{5}, EFrequency::SemiAnnual, EStubType::ShortFirst, EDateRule::ModFollowing, holidays, EDayCount::Actual_d365, 1e6, 6.22 / 100, days{2}, 0.0);
    auto swap7Y = IrSwap(spotDate, years{7}, EFrequency::SemiAnnual, EStubType::ShortFirst, EDateRule::ModFollowing, holidays, EDayCount::Actual_d365, 1e6, 6.32 / 100, days{2}, 0.0);
    auto swap10Y = IrSwap(spotDate, years{10}, EFrequency::SemiAnnual, EStubType::ShortFirst, EDateRule::ModFollowing, holidays, EDayCount::Actual_d365, 1e6, 6.42 / 100, days{2}, 0.0);
    auto swap15Y = IrSwap(spotDate, years{15}, EFrequency::SemiAnnual, EStubType::ShortFirst, EDateRule::ModFollowing, holidays, EDayCount::Actual_d365, 1e6, 6.56 / 100, days{2}, 0.0);
    auto swap20Y = IrSwap(spotDate, years{20}, EFrequency::SemiAnnual, EStubType::ShortFirst, EDateRule::ModFollowing, holidays, EDayCount::Actual_d365, 1e6, 6.56 / 100, days{2}, 0.0);
    auto swap30Y = IrSwap(spotDate, years{30}, EFrequency::SemiAnnual, EStubType::ShortFirst, EDateRule::ModFollowing, holidays, EDayCount::Actual_d365, 1e6, 6.56 / 100, days{2}, 0.0);
    auto swaps = std::vector<IrSwap> { swap2Y, swap3Y, swap4Y, swap5Y, swap7Y, swap10Y, swap15Y, swap20Y, swap30Y};

    auto yieldCurve1 = YieldCurve("solved", valueDate, deposits, futures, swaps, EDayCount::Actual_d365, EInterpolationMethod::CubicSpline);

    auto df1 = yieldCurve1.discountFactor(2000y/January/1d);
    REQUIRE ( fabs(df1 - 0.87486413978007949) < 1e-12 );
}