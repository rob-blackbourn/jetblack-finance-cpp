#ifndef __jetblack__maths__hermite_interp_hpp
#define __jetblack__maths__hermite_interp_hpp

#include <stdexcept>
#include <vector>

#include "maths/interp.hpp"
#include "maths/linear_interp.hpp"

namespace maths
{
	struct HermiteInterp : Interp
    {
		HermiteInterp()
			:	Interp()
		{
		}

        HermiteInterp(const std::vector<double>& xa, const std::vector<double>& ya, bool force_interpolation, bool extrapolate_near_flat, bool extrapolate_far_flat)
			:	Interp(xa, ya, force_interpolation, extrapolate_near_flat, extrapolate_far_flat)
        {
        }

		HermiteInterp(const HermiteInterp& rhs)
			:	Interp(rhs)
		{
		}

        virtual ~HermiteInterp() {}

        virtual double interpolate(double x) const
        {
            return interpolate(x, xa, ya, force_interpolation, extrapolate_near_flat, extrapolate_far_flat);
        }

		static double interpolate(double x, const std::vector<double>& xa, const std::vector<double>& ya, bool force_interpolation, bool extrapolate_near_flat, bool extrapolate_far_strat)
        {
            if (ya.size() != xa.size())
				throw std::invalid_argument("the input vectors must be the same length");
            size_t n = ya.size();

            size_t i = 0;
            size_t j = n - 1;

            // outside bounds
            if (x < xa[i]) return ya[i];
            if (x > xa[j]) return ya[j];

            //find strikes via bisection
            while (j != i + 1)
            {
                int k = ((i + j) / 2);
                if (x > xa[k])
                    i = k;
                else if (x < xa[k])
                    j = k;
                else
                    return ya[k];
            }

            //linear interp if must
            if (x < xa[1])
            {
				return LinearInterp::interpolate(x, xa[0], ya[0], xa[1], ya[1]);
            }
            else if (x > xa[j - 1])
            {
				return LinearInterp::interpolate(x, xa[j-1], ya[j-1], xa[j], ya[j]);
            }
			else
			{
				// Hermite with 2 points either side
				double d0 = (ya[i] - ya[i - 1]) / (xa[i] - xa[i - 1]);
				double d1 = ((ya[i + 1] - ya[i]) / (xa[i + 1] - xa[i]) - d0) / (xa[i + 1] - xa[i - 1]);
				double d2 = (((ya[i + 2] - ya[i + 1]) / (xa[i + 2] - xa[i + 1]) - (ya[i + 1] - ya[i]) / (xa[i + 1] - xa[i])) / (xa[i + 2] - xa[i]) - d1) / (xa[i + 2] - xa[i - 1]);

				return ya[i - 1] + d0 * (x - xa[i - 1]) + d1 * (x - xa[i - 1]) * (x - xa[i]) + d2 * (x - xa[i - 1]) * (x - xa[i]) * (x - xa[i + 1]);
			}
        }
    };
}

#endif // __jetblack__maths__hermite_interp_hpp
