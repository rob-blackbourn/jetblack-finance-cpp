#ifndef __jetblack__rates__instrument_hpp
#define __jetblack__rates__instrument_hpp

#include <chrono>
#include <memory>

namespace rates
{
	using namespace std::chrono;

    class YieldCurve;

    class Instrument
    {
    public:
        virtual ~Instrument()
        {            
        }

		virtual const year_month_day& endDate() const = 0;

		virtual double rate() const = 0;
		virtual void rate(double rate) = 0;

		virtual double solveZeroRate(YieldCurve& curve, size_t index) = 0;

        virtual std::shared_ptr<Instrument> clone_shared() const = 0;
        virtual std::unique_ptr<Instrument> clone_unique() const = 0;
    };
}

#endif // __jetblack__rates__instrument_hpp
