#ifndef __jetblack__NR__RootFinder_hpp
#define __jetblack__NR__RootFinder_hpp

#include <vector>
#include <cmath>

#pragma warning( disable:4290 )

namespace NR
{
	
	class RootFinder  
	{
	public:	
		struct RootOF
		{
			virtual double operator()(double) = 0;
		};

	public:
		RootFinder(double errorTolerance = 1e-15, int maxIterations = 1000, double machinePrecision = 1e-10)
			:	errorTolerance(errorTolerance),
				maxIterations(maxIterations),
				machinePrecision(machinePrecision)
		{
		}
		RootFinder(const RootFinder& rootFinder)
			:	errorTolerance(rootFinder.errorTolerance),
				maxIterations(rootFinder.maxIterations),
				machinePrecision(rootFinder.machinePrecision)
		{
		}
		virtual ~RootFinder()
		{
		}
	 
		double findRoot(RootOF& of, double x1, double x2)
		{
			int iter;
			double a=x1,b=x2,c=x2;
			double d,e,min1,min2;
			double fc,p,q,r,s,tol1,xm;
			double fa,fb;

			fa = of(a);
			fb = of(b);

			if ((fa > 0.0 && fb > 0.0) || (fa < 0.0 && fb < 0.0))
			{
				throw std::runtime_error("root must be bracketed in rootFinder");
			}
			
			fc = fb;
			for (iter = 1; iter <= maxIterations; ++iter)
			{
				if ((fb > 0.0 && fc > 0.0) || (fb < 0.0 && fc < 0.0))
				{
					c=a;
					fc=fa;
					e=d=b-a;
				}
				if (fabs(fc) < fabs(fb))
				{
					a=b;
					b=c;
					c=a;
					fa=fb;
					fb=fc;
					fc=fa;
				}

				tol1 = 2.0 * machinePrecision * fabs(b) + 0.5 * errorTolerance;
				xm=0.5*(c-b);
				if (fabs(xm) <= tol1 || fb == 0.0)
				{
					return b;
				}
				if (fabs(e) >= tol1 && fabs(fa) > (fb))
				{
					s=fb/fa;
					if (a==c)
					{
						p=2.0*xm*s;
						q=1.0-s;
					}
					else
					{	
						q=fa/fc;
						r=fb/fc;
						p=s*(2.0*xm*q*(q-r)-(b-a)*(r-1.0));
						q=(q-1.0)*(r-1.0)*(s-1.0);
					}
					if (p > 0.0)
						q = -q;

					p=fabs(p);
					min1=3.0*xm*q-fabs(tol1*q);
					min2=fabs(e*q);
					if (2.0*p < (min1 < min2 ? min1 : min2 ))
					{
						e=d;
						d=p/q;
					}
					else
					{
						d=xm;
						e=d;
					}
				}
				else
				{
					d=xm;
					e=d;
				}
				a=b;
				fa=fb;
				if (fabs(d) > tol1)
					b += d;
				else
					b += sign(tol1,xm);

				fb = of(b);
			}

			throw std::runtime_error("number of iterations exceeded in findRoot");
		}



		double sign(double a, double b) { return b >= 0.0 ? fabs(a) : -fabs(a); };

		double getErrorTolerance() const { return errorTolerance; }
		void setErrorTolerance(double errorTolerance) { this->errorTolerance = errorTolerance; }

		double getMachinePrecision() const { return machinePrecision; }
		void setMachinePrecision(double machinePrecision) { this->machinePrecision = machinePrecision; }

		int getMaxIterations() const { return maxIterations; }
		void setMaxIterations(int maxIterations) { this->maxIterations = maxIterations; }

		bool findBracket(RootOF& of, double &x1, double &x2)
		{
			if (x1 == x2)
				throw std::invalid_argument("Bad initial range in zbrac");

			const unsigned int NTRY = 50;
			const double FACTOR = 1.6;

			double f1 = of(x1);
			double f2 = of(x2);
			for (unsigned int j = 0; j < NTRY; ++j)
			{
				if (f1 * f2 < 0.0)
					return true;
				if (fabs(f1) < fabs(f2))
					f1 = of(x1 += FACTOR * (x1 - x2));
				else
					f2 = of(x2 += FACTOR * (x2 - x1));
			}

			return false;
		}

		std::vector<double>::size_type reduceBracket(RootOF& of, double x1, double x2, unsigned int n, std::vector<double>& xb1, std::vector<double>& xb2)
		{
			std::vector<double>::size_type nb = xb1.size();
			std::vector<double>::size_type nroot = 0;
			double dx = (x2 - x1) / n;
			double x = x1;
			double fp = of(x);
			for (unsigned int i = 0; i < n; ++i)
			{
				double fc = of(x += dx);
				if (fc * fp <= 0.0)
				{
					xb1[nroot] = x - dx;
					xb2[nroot++] = x;
					if(nroot == nb)
						return nroot;
				}
				fp = fc;
			}
			return nroot;
		}

	private:
		double	errorTolerance;
		int		maxIterations;
		double	machinePrecision;
	};
}

#endif __jetblack__NR__RootFinder_hpp
