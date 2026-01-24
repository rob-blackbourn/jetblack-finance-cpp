#ifndef __jetblack__rates__dated_rate_hpp
#define __jetblack__rates__dated_rate_hpp

#include <chrono>

namespace rates
{
	using namespace std::chrono;

	class DatedRate
	{
	public:

		DatedRate() = default;

		DatedRate(year_month_day date, double rate)
			:	date_(date),
				rate_(rate)
		{
		}

		double rate() const { return rate_; }
		void rate(double r) { rate_ = r; }

		const year_month_day& date() const { return date_; }
		void date(const year_month_day& d) { date_ = d; }

		bool operator==(const DatedRate&) const = default;

	private:
		year_month_day date_ {};
		double rate_ {};
	};
}

#endif // __jetblack__rates__dated_rate_hpp
