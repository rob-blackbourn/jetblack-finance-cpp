#ifndef __jetblack__NR__SplineInterp_hpp
#define __jetblack__NR__SplineInterp_hpp

#include <vector>

#include "NR/NR.hpp"

namespace NR
{
	struct SplineInterpCurve
	{
		SplineInterpCurve() {}
		SplineInterpCurve(const std::vector< std::pair<double,double> >& _curve, const double _yp1, const double _ypn)
			:	curve(_curve),
				yp1(_yp1),
				ypn(_ypn)
		{
			y2axis = spline(curve, yp1, ypn);
		}

		SplineInterpCurve(const SplineInterpCurve& spline_interp)
			:	curve(spline_interp.curve),
				yp1(spline_interp.yp1),
				ypn(spline_interp.ypn),
				y2axis(spline_interp.y2axis)
		{
		}

		double interpolate(double x) const { return splint(curve, y2axis, x); }

		std::vector< std::pair<double,double> > curve;
		std::vector<double> y2axis;
		double yp1, ypn;
	};

}

#endif // __jetblack__NR__SplineInterp_hpp
