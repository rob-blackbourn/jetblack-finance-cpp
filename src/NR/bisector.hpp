#ifndef __jetblack__NR__bisector2_hpp
#define __jetblack__NR__bisector2_hpp

#include <cmath>
#include <concepts>
#include <functional>
#include <stdexcept>

namespace NR
{
	namespace bisector
	{
		template <typename F>
		requires std::invocable<F&, double>
		double solve(F of, double lox, double hix, int maxIterations, double errorTolerance)
		{
			double lof = of(lox);
			double hif = of(hix);

			if ((lof > 0.0 && hif > 0.0) ||
				(lof < 0.0 && hif < 0.0))
				return false;

			bool usual = true;
			if (lof > 0.0) usual = false;

			for (int i = 0; i < maxIterations; i++)
			{
				double x = (hix + lox) / 2.0;
				double f = of(x);

				if (fabs(f - 0.0) < errorTolerance)
					return x;

				if (f < 0)	
				{
					if (usual) lox = x;
					else	   hix = x;
				}
				else
				{
					if (usual) hix = x;
					else	   lox = x;
				}
			}

			throw std::runtime_error("unable to solve");
		}
	}
}

#endif // __jetblack__NR__bisector2_hpp
