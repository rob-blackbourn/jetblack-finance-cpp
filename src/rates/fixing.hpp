#ifndef __jetblack__rates__dated_rate_hpp
#define __jetblack__rates__dated_rate_hpp

#include <chrono>
#include <optional>

namespace rates
{
	using namespace std::chrono;

	class Fixing
	{
	public:

		Fixing() = default;

		Fixing(year_month_day date, double rate)
			:	date_(date),
				rate_(rate)
		{
		}

		const std::optional<double>& rate() const { return rate_; }
		void rate(const std::optional<double>& r) { rate_ = r; }

		const year_month_day& date() const { return date_; }
		void date(const year_month_day& d) { date_ = d; }

		bool operator==(const Fixing&) const = default;

	private:
		year_month_day date_ {};
		std::optional<double> rate_ {};
	};
}

#endif // __jetblack__rates__dated_rate_hpp
