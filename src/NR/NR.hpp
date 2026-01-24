#ifndef __jetblack__NR__NR_hpp
#define __jetblack__NR__NR_hpp

#include <cmath>
#include <vector>

namespace NR
{
	inline double ran2(int &idum)
	{
		const int IM1=2147483563,IM2=2147483399;
		const int IA1=40014,IA2=40692,IQ1=53668,IQ2=52774;
		const int IR1=12211,IR2=3791,NTAB=32,IMM1=IM1-1;
		const int NDIV=1+IMM1/NTAB;
		const double EPS=3.0e-16,RNMX=1.0-EPS,AM=1.0/double(IM1);
		static int idum2=123456789,iy=0;
		static int iv[NTAB] = {
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0};
		int j,k;
		double temp;

		if (idum <= 0) {
			idum=(idum==0 ? 1 : -idum);
			idum2=idum;
			for (j=NTAB+7;j>=0;j--) {
				k=idum/IQ1;
				idum=IA1*(idum-k*IQ1)-k*IR1;
				if (idum < 0)
					idum += IM1;
				if (j < NTAB)
					iv[j] = idum;
			}
			iy=iv[0];
		}
		k=idum/IQ1;
		idum=IA1*(idum-k*IQ1)-k*IR1;
		if (idum < 0) idum += IM1;
		k=idum2/IQ2;
		idum2=IA2*(idum2-k*IQ2)-k*IR2;
		if (idum2 < 0) idum2 += IM2;
		j=iy/NDIV;
		iy=iv[j]-idum2;
		iv[j] = idum;
		if (iy < 1) iy += IMM1;
		if ((temp=AM*iy) > RNMX) return RNMX;
		else return temp;
	}

	inline std::vector<double> spline(const std::vector< std::pair<double,double> >& curve, const double yp1, const double ypn)
	{
		size_t n = curve.size();
		if (n < 2)
			throw std::invalid_argument("the curve must have at least two points");

		std::vector<double> y2(n);

		std::vector<double> u(n);
		if (yp1 > 0.99e30)
			y2[0] = u[0] = 0.0;
		else
		{
			y2[0] = -0.5;
			u[0] = (3.0 / (curve[1].first - curve[0].first)) * ((curve[1].second - curve[0].second) / (curve[1].first - curve[0].first) - yp1);
		}

		for (unsigned int i = 1; i <= n - 2; ++i)
		{
			double sig = (curve[i].first - curve[i-1].first) / (curve[i+1].first - curve[i-1].first);
			double p = sig * y2[i-1] + 2.0;
			y2[i] = (sig - 1.0) / p;
			u[i] = (curve[i+1].second - curve[i].second) / (curve[i+1].first - curve[i].first) - (curve[i].second - curve[i-1].second) / (curve[i].first - curve[i-1].first);
			u[i] = (6.0 * u[i] / (curve[i+1].first - curve[i-1].first) - sig * u[i-1]) / p;
		}

		double qn, un;
		if (ypn > 0.99e30)
			qn = un = 0.0;
		else
		{
			qn = 0.5;
			un = (3.0 / (curve[n-1].first - curve[n-2].first)) * (ypn - (curve[n-1].second - curve[n-2].second) / (curve[n-1].first - curve[n-2].second));
		}

		y2[n-1] = (un - qn * u[n-2]) / (qn * y2[n-2] + 1.0);
		for (int k = static_cast<int>(n - 2); k >= 0; --k)
			y2[k] = y2[k] * y2[k+1] + u[k];

		return y2;
	}

	inline std::vector<double> spline(const std::vector<double>& xa, const std::vector<double>& ya, const double yp1, const double ypn)
	{
		if (xa.size() != ya.size())
			throw std::invalid_argument("input arrays must be the same length");

		size_t n = xa.size();
		if (n < 2)
			throw std::invalid_argument("the curve must have at least two points");

		std::vector<double> y2(n);

		std::vector<double> u(n);
		if (yp1 > 0.99e30)
			y2[0] = u[0] = 0.0;
		else
		{
			y2[0] = -0.5;
			u[0] = (3.0 / (xa[1] - xa[0])) * ((ya[1] - ya[0]) / (xa[1] - xa[0]) - yp1);
		}

		for (unsigned int i = 1; i <= n - 2; ++i)
		{
			double sig = (xa[i] - xa[i-1]) / (xa[i+1] - xa[i-1]);
			double p = sig * y2[i-1] + 2.0;
			y2[i] = (sig - 1.0) / p;
			u[i] = (ya[i+1] - ya[i]) / (xa[i+1] - xa[i]) - (ya[i] - ya[i-1]) / (xa[i] - xa[i-1]);
			u[i] = (6.0 * u[i] / (xa[i+1] - xa[i-1]) - sig * u[i-1]) / p;
		}

		double qn, un;
		if (ypn > 0.99e30)
			qn = un = 0.0;
		else
		{
			qn = 0.5;
			un = (3.0 / (xa[n-1] - xa[n-2])) * (ypn - (ya[n-1] - ya[n-2]) / (xa[n-1] - xa[n-2]));
		}

		y2[n-1] = (un - qn * u[n-2]) / (qn * y2[n-2] + 1.0);
		for (int k = static_cast<int>(n - 2); k >= 0; --k)
			y2[k] = y2[k] * y2[k+1] + u[k];

		return y2;
	}


	inline double splint(const std::vector< std::pair<double,double> >& curve, const std::vector<double>& y2a, const double x)
	{
		if (curve.size() != y2a.size())
			throw std::invalid_argument("the curve and y2a array must have the same length");

		size_t n = curve.size();
		if (n < 2)
			throw std::invalid_argument("the input arrays must have at least two points");

		int klo = 0, khi = static_cast<int>(n - 1);
		while (khi - klo > 1)
		{
			int k = (khi + klo) >> 1;
			if (curve[k].first > x)
				khi = k;
			else
				klo = k;
		}

		double h = curve[khi].first - curve[klo].first;

		if (h == 0.0)
			throw std::invalid_argument("Bad xa input to splint");

		double a = (curve[khi].first - x) / h;
		double b = (x - curve[klo].first) / h;
		return a * curve[klo].second + b * curve[khi].second + ((a * a * a - a) * y2a[klo] +(b * b * b - b) * y2a[khi]) * (h * h) / 6.0;
	}

	inline double splint(const std::vector<double>& xa, const std::vector<double>& ya, const std::vector<double>& y2a, const double x)
	{
		if (xa.size() != ya.size() || xa.size() != y2a.size())
			throw std::invalid_argument("the x array, y array, and y2a array must have the same length");

		size_t n = xa.size();
		if (n < 2)
			throw std::invalid_argument("the input arrays must have at least two points");

		int klo = 0, khi = static_cast<int>(n - 1);
		while (khi - klo > 1)
		{
			int k = (khi + klo) >> 1;
			if (xa[k] > x)
				khi = k;
			else
				klo = k;
		}

		double h = xa[khi] - xa[klo];

		if (h == 0.0)
			throw std::invalid_argument("Bad xa input to splint");

		double a = (xa[khi] - x) / h;
		double b = (x - xa[klo]) / h;
		return a * ya[klo] + b * ya[khi] + ((a * a * a - a) * y2a[klo] +(b * b * b - b) * y2a[khi]) * (h * h) / 6.0;
	}

	inline bool bisec(double func(double), double lox, double hix, double acc, double* x)
	{
		double lof = func(lox);
		double hif = func(hix);

		if ((lof > 0.0 && hif > 0.0) ||
			(lof < 0.0 && hif < 0.0))
			return false;

		bool usual = true;
		if (lof > 0.0) usual = false;

		// 50 iterations allowed here
		for (int i = 0; i < 50; i++)
		{
			*x = (hix + lox) / 2.0;
			double f = func(*x);

			if (fabs(f - 0.0) < acc) return true;

			if (f < 0)	
			{
				if (usual) lox = *x;
				else	   hix = *x;
			}
			else
			{
				if (usual) hix = *x;
				else	   lox = *x;
			}
		}

		return false;
	}

	inline bool rtsec(double (*func)(double), double x1, double x2, double xacc,double* root)
	// Using the secant method, find the root of a function func thought to lie between x1 and x2.
	// The root, returned as rtsec, is refined until its accuracy is +/-xacc.
	{
		int j;

		double fl, f, dx, swap, xl, rts;

		fl = (*func)(x1);
		f = (*func)(x2);
		if (fabs(fl) < fabs(f)) {		// Pick the bound with the smaller function value as
			rts = x1;					// the most recent guess.
			xl = x2;
			swap = fl;
			fl = f;
			f = swap;
		} else {
			xl = x1;
			rts = x2;
		}
		for (j = 1; j <= 30; j++) {				// Secant loop.
			dx = (xl - rts) * f / (f - fl);		// Increment with respect to latest value.
			xl = rts;
			fl = f;
			rts += dx;
			f = (*func)(rts);
			if (fabs(dx) < xacc || f == 0.0) 
			{
				*root = rts;
				return true;		// Convergence.
			}
		}
		return false;
	}
}

#endif // __jetblack__NR__NR_hpp
