#include "dates/terms.hpp"

#include <chrono>
#include <tuple>
#include <vector>

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

using namespace dates;

TEST_CASE("d30A_360", "[dates]")
{
    using namespace std::chrono;
    using test_data_t = std::tuple<year_month_day, year_month_day, days>;

    test_data_t data[] = {
        // Example 1: End dates do not involve the last day of February
        {2006y/August/20d,    2007y/February/20d, days{180}},
        {2007y/February/20d,  2007y/August/20d,   days{180}},
        {2007y/August/20d,    2008y/February/20d, days{180}},
        {2008y/February/20d,  2008y/August/20d,   days{180}},
        {2008y/August/20d,    2009y/February/20d, days{180}},
        {2009y/February/20d,  2009y/August/20d,   days{180}}, 

        // Example 2: End dates include some end-February dates
        {2006y/August/31d,    2007y/February/28d, days{178}},
        {2007y/February/28d,  2007y/August/31d,   days{183}},
        {2007y/August/31d,    2008y/February/29d, days{179}},
        {2008y/February/29d,  2008y/August/31d,   days{182}},
        {2008y/August/31d,    2009y/February/28d, days{178}},
        {2009y/February/28d,  2009y/August/31d,   days{183}},

        // Example 3: Miscellaneous calculations
        {2006y/January/31d,   2006y/February/28d, days{ 28}},
        {2006y/January/30d,   2006y/February/28d, days{ 28}},
        {2006y/February/28d,  2006y/March/3d,     days{  5}},
        {2006y/February/14d,  2006y/February/28d, days{ 14}},
        {2006y/September/30d, 2006y/October/31d,  days{ 30}},
        {2006y/October/31d,   2006y/November/28d, days{ 28}},
        {2007y/August/31d,    2008y/February/28d, days{178}},
        {2008y/February/28d,  2008y/August/28d,   days{180}},
        {2008y/February/28d,  2008y/August/30d,   days{182}},
        {2008y/February/28d,  2008y/August/31d,   days{183}},
        {2007y/February/26d,  2008y/February/28d, days{362}},
        {2007y/February/26d,  2008y/February/29d, days{363}},
        {2008y/February/29d,  2009y/February/28d, days{359}},
        {2008y/February/28d,  2008y/March/30d,    days{ 32}},
        {2008y/February/28d,  2008y/March/31d,    days{ 33}}
    };

    for (auto&& [date1, date2, expectedDays] : data)
    {
        auto&& [d, t] = dates::getTerm(date1, date2, EDayCount::d30A_360);
        REQUIRE( d == expectedDays );
    }
}

TEST_CASE("d30E_d360", "[dates]")
{
    using namespace std::chrono;
    using test_data_t = std::tuple<year_month_day, year_month_day, days>;

    test_data_t data[] = {
        // No end February dates.
        {2006y/August/20d,    2007y/February/20d, days{180}},
        {2007y/February/20d,  2007y/August/20d,   days{180}},
        {2007y/August/20d,    2008y/February/20d, days{180}},
        {2008y/February/20d,  2008y/August/20d,   days{180}},
        {2008y/August/20d,    2009y/February/20d, days{180}},
        {2009y/February/20d,  2009y/August/20d,   days{180}},

        // Some end-February dates
        {2006y/February/28d,  2006y/August/31d,   days{182}},
        {2006y/August/31d,    2007y/February/28d, days{178}},
        {2007y/February/28d,  2007y/August/31d,   days{182}},
        {2007y/August/31d,    2008y/February/29d, days{179}},
        {2008y/February/29d,  2008y/August/31d,   days{181}},
        {2008y/August/31d,    2009y/February/28d, days{178}},
        {2009y/February/28d,  2009y/August/31d,   days{182}},
        {2009y/August/31d,    2010y/February/28d, days{178}},
        {2010y/February/28d,  2010y/August/31d,   days{182}},
        {2010y/August/31d,    2011y/February/28d, days{178}},
        {2011y/February/28d,  2011y/August/31d,   days{182}},
        {2011y/August/31d,    2012y/February/29d, days{179}},

        // Regression tests
        {2006y/January/31d,   2006y/February/28d, days{ 28}},
        {2006y/January/30d,   2006y/February/28d, days{ 28}},
        {2006y/February/28d,  2006y/March/3d,     days{  5}},
        {2006y/February/14d,  2006y/February/28d, days{ 14}},
        {2006y/September/30d, 2006y/October/31d,  days{ 30}},
        {2006y/October/31d,   2006y/November/28d, days{ 28}},
        {2007y/August/31d,    2008y/February/28d, days{178}},
        {2008y/February/28d,  2008y/August/28d,   days{180}},
        {2008y/February/28d,  2008y/August/30d,   days{182}},
        {2008y/February/28d,  2008y/August/31d,   days{182}},
        {2007y/February/26d,  2008y/February/28d, days{362}},
        {2007y/February/26d,  2008y/February/29d, days{363}},
        {2008y/February/29d,  2009y/February/28d, days{359}},
        {2008y/February/28d,  2008y/March/30d,    days{ 32}},
        {2008y/February/28d,  2008y/March/31d,    days{ 32}}
    };

    for (auto&& [date1, date2, expectedDays] : data)
    {
        auto&& [d, t] = dates::getTerm(date1, date2, EDayCount::d30E_d360);
        REQUIRE( d == expectedDays );
    }
}

TEST_CASE("d30E_d360_ISDA", "[dates]")
{
    using namespace std::chrono;
    using test_data_t = std::tuple<year_month_day, year_month_day, year_month_day, days>;

    test_data_t data[] = {
        // End dates not the last day of February.
        {2006y/August/20d,    2007y/February/20d, 2009y/August/20d, days{180}},
        {2007y/February/20d,  2007y/August/20d,   2009y/August/20d, days{180}},
        {2007y/August/20d,    2008y/February/20d, 2009y/August/20d, days{180}},
        {2008y/February/20d,  2008y/August/20d,   2009y/August/20d, days{180}},
        {2008y/August/20d,    2009y/February/20d, 2009y/August/20d, days{180}},
        {2009y/February/20d,  2009y/August/20d,   2009y/August/20d, days{180}},

        // Some maturity dates lie on end dates.
        {2006y/February/28d,  2006y/August/31d,   2012y/February/29d, days{180}},
        {2006y/August/31d,    2007y/February/28d, 2012y/February/29d, days{180}},
        {2007y/February/28d,  2007y/August/31d,   2012y/February/29d, days{180}},
        {2007y/August/31d,    2008y/February/29d, 2012y/February/29d, days{180}},
        {2008y/February/29d,  2008y/August/31d,   2012y/February/29d, days{180}},
        {2008y/August/31d,    2009y/February/28d, 2012y/February/29d, days{180}},
        {2009y/February/28d,  2009y/August/31d,   2012y/February/29d, days{180}},
        {2009y/August/31d,    2010y/February/28d, 2012y/February/29d, days{180}},
        {2010y/February/28d,  2010y/August/31d,   2012y/February/29d, days{180}},
        {2010y/August/31d,    2011y/February/28d, 2012y/February/29d, days{180}},
        {2011y/February/28d,  2011y/August/31d,   2012y/February/29d, days{180}},
        {2011y/August/31d,    2012y/February/29d, 2012y/February/29d, days{179}},

        // Regression tests.
        {2006y/January/31d,   2006y/February/28d, 2008y/February/29d, days{ 30}},
        {2006y/January/30d,   2006y/February/28d, 2008y/February/29d, days{ 30}},
        {2006y/February/28d,  2006y/March/3d,     2008y/February/29d, days{  3}},
        {2006y/February/14d,  2006y/February/28d, 2008y/February/29d, days{ 16}},
        {2006y/September/30d, 2006y/October/31d,  2008y/February/29d, days{ 30}},
        {2006y/October/31d,   2006y/November/28d, 2008y/February/29d, days{ 28}},
        {2007y/August/31d,    2008y/February/28d, 2008y/February/29d, days{178}},
        {2008y/February/28d,  2008y/August/28d,   2008y/February/29d, days{180}},
        {2008y/February/28d,  2008y/August/30d,   2008y/February/29d, days{182}},
        {2008y/February/28d,  2008y/August/31d,   2008y/February/29d, days{182}},
        {2007y/February/28d,  2008y/February/28d, 2008y/February/29d, days{358}},
        {2007y/February/28d,  2008y/February/29d, 2008y/February/29d, days{359}},
        {2008y/February/29d,  2009y/February/28d, 2008y/February/29d, days{360}},
        {2008y/February/29d,  2008y/March/30d,    2008y/February/29d, days{ 30}},
        {2008y/February/29d,  2008y/March/31d,    2008y/February/29d, days{ 30}}

    };

    for (auto&& [date1, date2, maturity, expectedDays] : data)
    {
        auto&& [d, t] = dates::getTerm(date1, date2, EDayCount::d30E_d360_ISDA, maturity);
        REQUIRE( d == expectedDays );
    }
}

TEST_CASE("Actual_Actual", "[dates]")
{
    using namespace std::chrono;
    using test_data_t = std::tuple<year_month_day, year_month_day, days, double>;

    test_data_t data[] = {
        // 2020 is a leap year
        // Single year within one year.
        {2020y/January/1d,    2021y/January/1d, days{366}, 1.0},
        {2021y/January/1d,    2022y/January/1d, days{365}, 1.0},
        // Single year over two years.
        {2020y/January/30d,    2021y/January/30d, days{366}, 1.0002170821169249},
        {2021y/January/30d,    2022y/January/30d, days{365}, 1.0},
        // Half a year with no overlapping years.
        {2020y/January/1d,    2020y/July/1d, days{182}, 182 / 366.0},
        {2021y/January/1d,    2021y/July/1d, days{181}, 181 / 365.0},
        // Two years over two whole years.
        {2020y/January/1d,    2022y/January/1d, days{366 + 365}, 2.0},
        {2021y/January/1d,    2023y/January/1d, days{365 + 365}, 2.0},
        // Two years with overlapping years.
        {2020y/January/30d,    2022y/January/30d, days{366 + 365}, 2.0002170821169249},
        {2021y/January/30d,    2023y/January/30d, days{365 + 365}, 2.0},
        // Two years six months over two whole years.
        {2020y/January/1d,    2022y/July/1d, days{912}, 2.495890410958904},
        {2021y/January/1d,    2023y/July/1d, days{911}, 2.495890410958904},
        // Two years six months with overlapping years.
        {2020y/January/30d,    2022y/July/30d, days{912}, 2.4961074930758289},
        {2021y/January/30d,    2023y/July/30d, days{911}, 2.495890410958904},
    };

    for (auto&& [date1, date2, expectedDays, expectedTime] : data)
    {
        auto&& [d, t] = dates::getTerm(date1, date2, EDayCount::Actual_Actual);
        REQUIRE( d == expectedDays );
        REQUIRE( t == Approx(expectedTime).epsilon(1e-12) );
    }

}