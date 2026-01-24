#ifndef __jetblack__maths__spline_interp_hpp
#define __jetblack__maths__spline_interp_hpp

#include <vector>

#include "maths/interp.hpp"
#include "NR/NR.hpp"

namespace maths
{
	struct SplineIterp : public Interp
	{
		std::vector<double> y2axis;
		double yp1, ypn;

		SplineIterp()
			:	Interp()
		{
		}

		SplineIterp(const std::vector<double>& xa, const std::vector<double>& ya, bool force_interpolation, bool extrapolate_near_flat, bool extrapolate_far_flat, const double yp1, const double ypn)
			:	Interp(xa, ya, force_interpolation, extrapolate_near_flat, extrapolate_far_flat),
				yp1(yp1),
				ypn(ypn)
		{
			initialise();
		}

		SplineIterp(const SplineIterp& rhs)
			:	Interp(rhs),
				y2axis(rhs.y2axis),
				yp1(rhs.yp1),
				ypn(rhs.ypn)
		{
		}

		virtual ~SplineIterp() {}

		virtual double interpolate(double x) const
		{
			if (x < xa.front() && extrapolate_near_flat)
				return ya.front();
			else if (x > xa.back() && extrapolate_far_flat)
				return ya.back();
			else
			{
				if (!force_interpolation)
					for (size_t i = 0; i < xa.size(); ++i)
						if (xa[i] == x)
							return ya[i];

				return NR::splint(xa, ya, y2axis, x);
			}
		}

		virtual void add(double x, double y)
		{
			dynamic_cast<Interp*>(this)->add(x, y);
			initialise();
		}

	private:
		void initialise() { y2axis = NR::spline(xa, ya, yp1, ypn); }
	};
}

#endif // __jetblack__maths__spline_interp_hpp
