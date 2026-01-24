#ifndef __jetblack__maths__interp_hpp
#define __jetblack__maths__interp_hpp

#include <vector>

namespace maths
{
	class Interp
	{
	public:
		Interp()
		{
		}

		Interp(
			const std::vector<double>& xa,
			const std::vector<double>& ya,
			bool force_interpolation,
			bool extrapolate_near_flat,
			bool extrapolate_far_flat)
			:	xa(xa),
				ya(ya),
				force_interpolation(force_interpolation),
				extrapolate_near_flat(extrapolate_near_flat),
				extrapolate_far_flat(extrapolate_far_flat)
		{
		}

		Interp(const Interp& rhs)
			:	xa(rhs.xa),
				ya(rhs.ya),
				force_interpolation(rhs.force_interpolation),
				extrapolate_near_flat(rhs.extrapolate_near_flat),
				extrapolate_far_flat(rhs.extrapolate_far_flat)
		{
		}

		virtual ~Interp() {}

		virtual double interpolate(double x) const = 0;
		virtual void add(double x, double y)
		{
			xa.push_back(x);
			ya.push_back(y);
		}

		const std::vector<double>& get_xa() const { return xa; }
		const std::vector<double>& get_ya() const { return ya; }

	protected:
		std::vector<double> xa;
		std::vector<double> ya;
		bool force_interpolation; // require interpolation rather than accepting an exact match
        bool extrapolate_near_flat;
        bool extrapolate_far_flat;
	};
}

#endif // __jetblack__maths__interp_hpp
