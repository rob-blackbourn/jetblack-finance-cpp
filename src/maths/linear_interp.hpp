#ifndef __jetblack__maths__linear_interp_hpp
#define __jetblack__maths__linear_interp_hpp

#include <stdexcept>
#include <vector>

#include "maths/interp.hpp"

namespace maths
{
	struct LinearInterp : public Interp
    {
		LinearInterp()
			:	Interp()
		{
		}

		LinearInterp(const std::vector<double>& xa, const std::vector<double>& ya, bool force_interpolation, bool extrapolate_near_flat, bool extrapolate_far_flat)
			:	Interp(xa, ya, force_interpolation, extrapolate_near_flat, extrapolate_far_flat)
        {
        }

		LinearInterp(const LinearInterp& rhs)
			:	Interp(rhs)
        {
        }

		virtual ~LinearInterp() {}

        virtual double interpolate(double x) const
        {
            return interpolate(x, xa, ya, force_interpolation, extrapolate_near_flat, extrapolate_far_flat);
        }

		static double interpolate(double xk, double x1, double y1, double x2, double y2)
		{
			return y1 + (xk - x1) / (x2 - x1) * (y2 - y1);
		}

		static double interpolate(double xk, const std::vector<double>& xa, const std::vector<double>& ya, bool force_interpolation, bool extrapolate_near_flat, bool extrapolate_far_flat)
        {
            if (xa.size() == 0)
				throw std::invalid_argument("input vectors must not be empty");
            else if (xa.size() != ya.size())
				throw std::invalid_argument("input vectors must be the same length");

            size_t n = xa.size();

            if (n == 1)
                return ya[0];
			else if (xk <= xa.front())
			{
				if (extrapolate_near_flat)
					return ya.front();
				else
					return interpolate(xk, xa[0], ya[0], xa[1], ya[1]);
			}
			else if (xk >= xa.back())
			{
				if (extrapolate_far_flat)
					return ya.back();
				else
					return interpolate(xk, xa[n-2], ya[n-2], xa[n-1], ya[n-1]);
			}
			else
			{
				for (size_t i = 0; i < n-1; ++i)
				{
					if (xa[i] == xk && !force_interpolation)
						return ya[i];
					else if (xa[i] <= xk && xk < xa[i+1])
						return interpolate(xk, xa[i], ya[i], xa[i+1], ya[i+1]);
				}
			}

			throw std::runtime_error("sack the coder");
        }
    };
}

#endif // __jetblack__maths__linear_interp_hpp
