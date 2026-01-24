#include "stdext/match.hpp"

#include "dates/business_days.hpp"
#include "dates/adjustments.hpp"

#include <chrono>
#include <iostream>
#include <ranges>
#include <vector>

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

using namespace dates;
using namespace stdext;
using namespace std::chrono;

using time_unit_t = std::variant<days, weeks, months, years>;

year_month_day add(const year_month_day& date, const time_unit_t& offset, bool eom, EDateRule dateRule, const std::set<year_month_day>& holidays = {})
{
    return std::visit( match {
        [&date, &holidays](days i) { return addBusinessDays(date, i, holidays); },
        [&date, dateRule, &holidays](weeks i) { return addWeeks(date, i, dateRule, holidays); },
        [&date, eom, dateRule, &holidays](months i) { return addMonths(date, i, eom, dateRule, holidays); },
        [&date, eom, dateRule, &holidays](years i) { return addYears(date, i, eom, dateRule, holidays); },
    }, offset);
}

TEST_CASE("match", "[stdext]")
{

    auto startDate = 2000y/January/31d;
    auto offsets = std::vector<time_unit_t> {
        days(1),
        weeks{2},
        months{3},
        years{4}
    };

    auto expecteds = std::vector<year_month_day> {
        addBusinessDays(startDate, days(1), {}),
        addWeeks(startDate, weeks{2}, EDateRule::None, {}),
        addMonths(startDate, months{3}, true, EDateRule::None, {}),
        addYears(startDate, years{4}, true, EDateRule::None, {})
    };

    auto actual = std::vector<year_month_day>();
    for (const auto& [offset, expected] : std::views::zip(offsets, expecteds))
    {
        auto actual = add(startDate, offset, true, EDateRule::None, {});
        REQUIRE ( expected == actual );
    }

}