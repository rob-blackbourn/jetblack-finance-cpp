#ifndef __jetblack__NR__RootSecant_hpp
#define __jetblack__NR__RootSecant_hpp

#include <cmath>
#include <stdexcept>

namespace NR
{
	class RootSecant
	{
	public:	
		struct func
		{
			virtual double operator()(double) = 0;
		};

	public:
		RootSecant(double errorTolerance = 1e-15, unsigned int maxIterations = 30)
			:	errorTolerance_(errorTolerance),
				maxIterations_(maxIterations)
		{
		}

		RootSecant(const RootSecant& rootSecant)
			:	errorTolerance_(rootSecant.errorTolerance_),
				maxIterations_(rootSecant.maxIterations_)
		{
		}
	 
		double solve(func& of, double lowerBound, double upperBound)
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

			for (unsigned int j = 0; j < maxIterations_; ++j)
			{											// Secant loop.
				double dx = (xl - rts) * f / (f - fl);	// Increment with respect to latest value.
				xl = rts;
				fl = f;
				rts += dx;
				f = of(rts);
				if (fabs(dx) < errorTolerance_ || f == 0.0) 
				{
					return rts; // Convergence.
				}
			}

			throw std::runtime_error("unable to find root");
		}

		double errorTolerance() const { return errorTolerance_; }
		void errorTolerance(double errorTolerance) { this->errorTolerance_ = errorTolerance; }

		unsigned int maxIterations() const { return maxIterations_; }
		void maxIterations(unsigned int maxIterations) { this->maxIterations_ = maxIterations; }

	private:
		double			errorTolerance_;
		unsigned int	maxIterations_;
	};
}

#endif // __jetblack__NR__RootSecant_hpp
