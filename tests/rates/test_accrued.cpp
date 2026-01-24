#include "rates/accrued.hpp"
#include "dates/frequency.hpp"

#include <chrono>

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

using namespace dates;
using namespace rates;
using namespace std::chrono;

TEST_CASE("flatRate", "accrued")
{
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

    auto jan10 = accrued(
        2026y/January/10d,
        schedule,
        EDayCount::Actual_d365,
        0.05,
        100.0);

    REQUIRE (fabs(jan10 - 0.1232876712) < 1e-10);

    auto feb10 = accrued(
        2026y/February/10d,
        schedule,
        EDayCount::Actual_d365,
        0.05,
        100.0);

    REQUIRE (fabs(feb10 - 0.1232876712) < 1e-10);

    auto jan1 = accrued(
        2026y/January/1d,
        schedule,
        EDayCount::Actual_d365,
        0.05,
        100.0);

    REQUIRE (fabs(jan1 - 0.0) < 1e-10);

    auto jan31 = accrued(
        2026y/January/31d,
        schedule,
        EDayCount::Actual_d365,
        0.05,
        100.0);

    REQUIRE (fabs(jan31 - 0.4109589041) < 1e-10);

    auto nov10 = accrued(
        2026y/November/10d,
        schedule,
        EDayCount::Actual_d365,
        0.05,
        100.0);

    REQUIRE (fabs(nov10 - 0.12328767123287671) < 1e-10);

    auto dec31 = accrued(
        2026y/December/31d,
        schedule,
        EDayCount::Actual_d365,
        0.05,
        100.0);

    REQUIRE (fabs(dec31 - 0.0) < 1e-10);
}

TEST_CASE("fixings", "accrued")
{
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
    auto fixings = std::vector<double> {
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

    auto jan10 = accrued(
        2026y/January/10d,
        schedule,
        EDayCount::Actual_d365,
        fixings,
        100.0);

    REQUIRE (fabs(jan10 - 0.024657534246575342) < 1e-10);

    auto feb10 = accrued(
        2026y/February/10d,
        schedule,
        EDayCount::Actual_d365,
        fixings,
        100.0);

    REQUIRE (fabs(feb10 - 0.049315068493150684) < 1e-10);

    auto jan1 = accrued(
        2026y/January/1d,
        schedule,
        EDayCount::Actual_d365,
        fixings,
        100.0);

    REQUIRE (fabs(jan1 - 0.0) < 1e-10);

    auto jan31 = accrued(
        2026y/January/31d,
        schedule,
        EDayCount::Actual_d365,
        fixings,
        100.0);

    REQUIRE (fabs(jan31 - 0.082191780821917804) < 1e-10);

    auto nov10 = accrued(
        2026y/November/10d,
        schedule,
        EDayCount::Actual_d365,
        fixings,
        100.0);

    REQUIRE (fabs(nov10 - 0.27123287671232876) < 1e-10);

    auto dec31 = accrued(
        2026y/December/31d,
        schedule,
        EDayCount::Actual_d365,
        fixings,
        100.0);

    REQUIRE (fabs(dec31 - 0.0) < 1e-10);

}