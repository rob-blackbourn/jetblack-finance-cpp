#include "rates/yield_curve.hpp"
#include "rates/deposit.hpp"
#include "rates/ir_future.hpp"
#include "rates/ir_swap.hpp"

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

    auto depositON = std::make_shared<Deposit>(1e6, 5.58675 / 100, valueDate, days{1}, EDayCount::Actual_d360, EDateRule::Following, holidays);
    auto depositTN = std::make_shared<Deposit>(1e6, 5.59375 / 100, depositON->maturity(), days{1}, EDayCount::Actual_d360, EDateRule::Following, holidays);
    auto deposit1M = std::make_shared<Deposit>(1e6, 5.625 / 100, spotDate, months{1}, EDayCount::Actual_d360, EDateRule::Following, holidays);
    auto deposit3M = std::make_shared<Deposit>(1e6, 5.71875 / 100, spotDate, months{3}, EDayCount::Actual_d360, EDateRule::Following, holidays);

    auto futureDec97 = std::make_shared<IrFuture>(1e6, 100 - 5.76, 1997y/December, EDayCount::Actual_d365, EDateRule::Following, daysToSpot, holidays);
    auto futureMar98 = std::make_shared<IrFuture>(1e6, 100 - 5.77, 1998y/March, EDayCount::Actual_d365, EDateRule::Following, daysToSpot, holidays);
    auto futureJun98 = std::make_shared<IrFuture>(1e6, 100 - 5.82, 1998y/June, EDayCount::Actual_d365, EDateRule::Following, daysToSpot, holidays);
    auto futureSep98 = std::make_shared<IrFuture>(1e6, 100 - 5.88, 1998y/September, EDayCount::Actual_d365, EDateRule::Following, daysToSpot, holidays);
    auto futureDec98 = std::make_shared<IrFuture>(1e6, 100 - 6.00, 1998y/December, EDayCount::Actual_d365, EDateRule::Following, daysToSpot, holidays);

    auto swap2Y = std::make_shared<IrSwap>(1e6, 6.01253 / 100, 0.0, spotDate, years{2}, EFrequency::SemiAnnual, EStubType::ShortFirst, EDateRule::ModFollowing, EDayCount::Actual_d365, days{2}, holidays);
    auto swap3Y = std::make_shared<IrSwap>(1e6, 6.10823 / 100, 0.0, spotDate, years{3}, EFrequency::SemiAnnual, EStubType::ShortFirst, EDateRule::ModFollowing, EDayCount::Actual_d365, days{2}, holidays);
    auto swap4Y = std::make_shared<IrSwap>(1e6, 6.16 / 100, 0.0, spotDate, years{4}, EFrequency::SemiAnnual, EStubType::ShortFirst, EDateRule::ModFollowing, EDayCount::Actual_d365, days{2}, holidays);
    auto swap5Y = std::make_shared<IrSwap>(1e6, 6.22 / 100, 0.0, spotDate, years{5}, EFrequency::SemiAnnual, EStubType::ShortFirst, EDateRule::ModFollowing, EDayCount::Actual_d365, days{2}, holidays);
    auto swap7Y = std::make_shared<IrSwap>(1e6, 6.32 / 100, 0.0, spotDate, years{7}, EFrequency::SemiAnnual, EStubType::ShortFirst, EDateRule::ModFollowing, EDayCount::Actual_d365, days{2}, holidays);
    auto swap10Y = std::make_shared<IrSwap>(1e6, 6.42 / 100, 0.0, spotDate, years{10}, EFrequency::SemiAnnual, EStubType::ShortFirst, EDateRule::ModFollowing, EDayCount::Actual_d365, days{2}, holidays);
    auto swap15Y = std::make_shared<IrSwap>(1e6, 6.56 / 100, 0.0, spotDate, years{15}, EFrequency::SemiAnnual, EStubType::ShortFirst, EDateRule::ModFollowing, EDayCount::Actual_d365, days{2}, holidays);
    auto swap20Y = std::make_shared<IrSwap>(1e6, 6.56 / 100, 0.0, spotDate, years{20}, EFrequency::SemiAnnual, EStubType::ShortFirst, EDateRule::ModFollowing, EDayCount::Actual_d365, days{2}, holidays);
    auto swap30Y = std::make_shared<IrSwap>(1e6, 6.56 / 100, 0.0, spotDate, years{30}, EFrequency::SemiAnnual, EStubType::ShortFirst, EDateRule::ModFollowing, EDayCount::Actual_d365, days{2}, holidays);

    auto instruments = std::vector<std::shared_ptr<Instrument>> {
        depositON, depositTN, deposit1M, deposit3M,
        futureDec97, futureMar98, futureJun98, futureSep98, futureDec98,
        swap2Y, swap3Y, swap4Y, swap5Y, swap7Y, swap10Y, swap15Y, swap20Y, swap30Y
    };

    auto yieldCurve1 = YieldCurve(valueDate, instruments, EDayCount::Actual_d365, EInterpolationMethod::CubicSpline);

    auto df1 = yieldCurve1.discountFactor(2000y/January/1d);
    REQUIRE ( df1 == Approx(0.87484318856686527).epsilon(1e-12) );
}