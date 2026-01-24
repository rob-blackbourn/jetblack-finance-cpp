#ifndef __jetblack__maths__brent_hpp
#define __jetblack__maths__brent_hpp

#include <cmath>
#include <concepts>
#include <functional>
#include <limits>
#include <stdexcept>

namespace maths
{
    namespace brent
    {
		template <typename F>
		requires std::invocable<F&, double>
        double solve(
            F of,
            double xMin,
            double xMax,
            unsigned int maxIterations = 100,
            double errorThreshold = std::numeric_limits<double>::epsilon())
        {
            const double machinePrecision = std::numeric_limits<double>::epsilon();

            double sa = xMin;
            double sb = xMax;
            double fa = of(sa);
            double fb = of(sb);

            double c = sa;
            double fc = fa;
            double e = sb - sa;
            double d = e;

            for (auto iteration = 0; iteration < maxIterations; ++iteration)
            {
                if (fabs(fc) < fabs(fb))
                {
                    sa = sb;
                    sb = c;
                    c = sa;
                    fa = fb;
                    fb = fc;
                    fc = fa;
                }

                // Convergence check.
                double tol = 2.0 * machinePrecision * fabs(sb) + errorThreshold;
                double m = 0.5 * (c - sb);

                if (fabs(m) <= tol || fb == 0.0)
                {
                    return sb;
                }

                if (fabs(e) < tol || fabs(fa) <= fabs(fb))
                {
                    e = m;
                    d = e;
                }
                else
                {
                    double p;
                    double q;

                    // Attempt inverse quadratic interpolation.
                    double s = fb / fa;

                    if (sa == c)
                    {
                        p = 2.0 * m * s;
                        q = 1.0 - s;
                    }
                    else
                    {
                        q = fa / fc;
                        auto r = fb / fc;
                        p = s * (2.0 * m * q * (q - r) - (sb - sa) * (r - 1.0));
                        q = (q - 1.0) * (r - 1.0) * (s - 1.0);
                    }

                    // Check whether in bounds
                    if (p > 0.0)
                    {
                        q = -q;
                    }
                    else
                    {
                        p = -p;
                    }

                    s = e;
                    e = d;

                    if (2.0 * p < 3.0 * m * q - fabs(tol * q) && p < fabs(0.5 * s * q))
                    {
                        // Accept interpolation.
                        d = p / q;
                    }
                    else
                    {
                        // Bounds decreasing too slowly, use bisection.
                        e = m;
                        d = e;
                    }
                }
                sa = sb;
                fa = fb;

                if (tol < fabs(d))
                {
                    sb = sb + d;
                }
                else if (0.0 < m)
                {
                    sb = sb + tol;
                }
                else
                {
                    sb = sb - tol;
                }

                fb = of(sb);

                if ((0.0 < fb && 0.0 < fc) || (fb <= 0.0 && fc <= 0.0))
                {
                    c = sa;
                    fc = fa;
                    e = sb - sa;
                    d = e;
                }
            }

            throw std::range_error("Failed to find root");
        }
    }
}

#endif // __jetblack__maths__brent_hpp
