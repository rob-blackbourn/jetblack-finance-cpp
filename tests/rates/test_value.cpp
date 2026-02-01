#include "rates/value.hpp"
#include "rates/yield_curve.hpp"

#include "dates/schedules.hpp"

#include <chrono>
#include <optional>
#include <ranges>

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

using namespace dates;
using namespace rates;
using namespace std::chrono;

TEST_CASE("couponRate/yieldCurve", "[value]")
{
    auto valueDate = 2026y/January/9d;
    auto curve = YieldCurve{0.05, valueDate, EDayCount::Actual_d365};
    auto schedule = std::vector<year_month_day> {
        2026y/January/1d,
        2026y/February/1d,
        2026y/March/1d,
        2026y/April/1d,
        2026y/May/1d,
        2026y/June/1d,
        2026y/July/1d,
        2026y/August/1d,
        2026y/September/1d,
        2026y/October/1d,
        2026y/November/1d,
        2026y/December/1d,
    };
    auto dayCount = EDayCount::Actual_d365;
    auto couponRate = 0.08;
    auto notional = 1000000.0;

    auto actual = rates::value(
        valueDate,
        curve,
        schedule,
        dayCount,
        couponRate,
        notional
    );
    REQUIRE ( actual == Approx(1027810.4346810074).epsilon(1e-10) );
}

TEST_CASE("couponRate/yieldRate", "[value]")
{
    auto valueDate = 2026y/January/9d;
    auto yieldRate = 0.05;
    auto schedule = std::vector<year_month_day> {
        2026y/January/1d,
        2026y/February/1d,
        2026y/March/1d,
        2026y/April/1d,
        2026y/May/1d,
        2026y/June/1d,
        2026y/July/1d,
        2026y/August/1d,
        2026y/September/1d,
        2026y/October/1d,
        2026y/November/1d,
        2026y/December/1d,
    };
    auto dayCount = EDayCount::Actual_d365;
    auto couponRate = 0.08;
    auto notional = 1000000.0;

    auto actual = rates::value(
        valueDate,
        yieldRate,
        schedule,
        dayCount,
        couponRate,
        notional,
        EFrequency::Monthly
    );
    REQUIRE ( actual == Approx(1027902.6838747344).epsilon(1e-10) );
}

TEST_CASE("fixings/yieldCurve", "[value]")
{
    auto valueDate = 2026y/January/9d;
    auto curve = YieldCurve{0.05, valueDate, EDayCount::Actual_d365};
    auto schedule = std::vector<year_month_day> {
        2026y/January/1d,
        2026y/February/1d,
        2026y/March/1d,
        2026y/April/1d,
        2026y/May/1d,
        2026y/June/1d,
        2026y/July/1d,
        2026y/August/1d,
        2026y/September/1d,
        2026y/October/1d,
        2026y/November/1d,
        2026y/December/1d,
    };
    auto dayCount = EDayCount::Actual_d365;
    auto fixingRates = std::vector<double> {
        0.01,
        0.02,
        0.03,
        0.04,
        0.05,
        0.06,
        0.07,
        0.08,
        0.09,
        0.10,
        0.11,
    };
    auto fixings = std::views::zip(
            schedule | std::views::drop(1),
            fixingRates)
        | std::views::transform([](auto&& t)
            {
                auto&& [date, rate] = t;
                return Fixing { date, rate };
            })
        | std::ranges::to<std::vector<Fixing>>();
    auto notional = 1000000.0;

    auto actual = rates::value(
        valueDate,
        curve,
        schedule,
        dayCount,
        fixings,
        notional
    );
    REQUIRE ( actual == Approx(1009728.5234190911).epsilon(1e-10) );
}
