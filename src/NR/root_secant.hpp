#ifndef __jetblack__NR__root_secant_hpp
#define __jetblack__NR__root_secant_hpp

#include <cmath>
#include <concepts>
#include <functional>
#include <stdexcept>

namespace NR
{
	namespace root_secant
	{
		template <typename F>
		requires std::invocable<F&, double>
		double solve(F of, double lowerBound, double upperBound, int maxIterations, double errorTolerance)
		{
			double xl, rts;

			double fl = of(lowerBound);
			double f = of(upperBound);

			if (fabs(fl) < fabs(f))
			{											// Pick the bound with the smaller function value as
				rts = lowerBound;						// the most recent guess.
				xl = upperBound;
				double swap = fl;
				fl = f;
				f = swap;
			}
			else
			{
				xl = lowerBound;
				rts = upperBound;
			}

			for (unsigned int j = 0; j < maxIterations; ++j)
			{											// Secant loop.
				double dx = (xl - rts) * f / (f - fl);	// Increment with respect to latest value.
				xl = rts;
				fl = f;
				rts += dx;
				f = of(rts);
				if (fabs(dx) < errorTolerance || f == 0.0) 
				{
					return rts; // Convergence.
				}
			}

			throw std::runtime_error("unable to find root");
		}
	}
}

#endif // __jetblack__NR__root_secant_hpp
