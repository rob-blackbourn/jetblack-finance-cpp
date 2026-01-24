#ifndef __jetblack__dates__festivals_hpp
#define __jetblack__dates__festivals_hpp

#include <cstring>

#include <algorithm>
#include <chrono>
#include <set>
#include <string>
#include <type_traits>
#include <vector>

namespace dates
{
    using namespace std::chrono;

	// <summary>
	// This function returns the date of easter in the given year.
	// </summary>
	// <param name="year">The year in question</param>
	// <returns>The date of easter</returns>
	inline
	year_month_day
	easter(year year)
	{
		// Note: Only true for gregorian dates
		int y, g, c, x, z, d, e, n, e1, n1, dpn;
	   
		y = static_cast<int>(year);
		g = (y - ((y / 19L) * 19L)) + 1L;
		c = (y / 100L) + 1L;
		x = ((3L * c) / 4L) - 12L;
		z = (((8L * c) + 5L) / 25L) - 5L;
		d = ((5L * y) / 4L) - x - 10L;
		e1 = ((11L * g) + 20L + z - x);
		e = e1 - ((e1 / 30L) * 30L);

		// The value of 'e' may be negative. The case of year = 14250, e.g.,
		// produces values of g = 1, z = 40 and x = 95. The value of e1 is thus
		// -24, and the 'mod' code fails to return the proper positive result.
		// The following correction produces a positive value, mod 30, for 'e'.
	      
		while (e < 0L) e += 30L;
	   
		if ( ((e == 25L) && (g > 11)) || (e == 24L) ) e++;

		n = 44L - e;

		if (n < 21L) n += 30L;
	  
		dpn = d + n;
		n1 = dpn - ((dpn / 7L) * 7L);   
		n = n + 7L - n1;

        month m;
		int day;
		if (n > 31L) {
			m = April;
			day = n - 31;
		}
		else {
			m = March;
			day = n;
		}
	   
		return year_month_day {year / m / day};
	}
}

#endif // __jetblack__dates__festivals_hpp
