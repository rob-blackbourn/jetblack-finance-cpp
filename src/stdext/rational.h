#pragma once

namespace stdext
{

	template<typename IntType>
	IntType
	GCD(IntType n, IntType m)
	{
		if (0 > n)
			n = -n;
		if (0 > m)
			m = -m;

		for(;;)
		{
			if (0 == m)
				return n;
			n %= m;
			if (0 == n)
				return m;
			m %= n;
		}
	}

	template <typename IntType>
	class rational
	{
	public:
		typedef IntType value_type;

	public:

		// comparason
		friend bool operator== (const rational& f, const rational& g)
		{
			return (f.numerator == g.numerator) && (f.denominator == g.denominator);
		}

		friend bool operator> (const rational& f, const rational& g)
		{
			return f.numerator * g.denominator > g.numerator * f.denominator;
		}

		// stream
		friend std::ostream& operator<< (std::ostream& output, const rational& f)
		{
			output << f.numerator << "/" << f.denominator;
			return output;
		}

		friend std::istream& operator>> (std::istream& input, rational& f)
		{
			input >> f.numerator;
			input >> f.denominator;
			assert(f.denominator != IntType(0));
			f.FixSign();
			f.Reduce();
			return input;
		}

	public:
		// construction
	//	rational() : numerator(IntType(0)), denominator(IntType(1)) {}
	//	rational(IntType n) : numerator(n), denominator(IntType(1)) {}
		rational(IntType n = IntType(0), IntType d = IntType(1)) : numerator(n), denominator(d)
		{
			assert(d != IntType(0));
			FixSign();
			Reduce();
		}

		// copy constructor
		rational(const rational& r)
		{
			numerator = r.numerator;
			denominator = r.denominator;
		}
		~rational() { }

		// assignment
		rational& operator= (const rational& r)
		{
			numerator = r.numerator;
			denominator = r.denominator;
			return *this;
		}

		// arithmatic
		rational operator+() const	// unary plus;
		{
			return *this;
		}

		rational operator-() const	// unary minus;
		{
			rational f = *this;
			f.numerator = -f.numerator;
			return f;
		}

		rational& operator+= (const rational& r)
		{
			numerator = numerator * r.denominator + denominator * r.numerator;
			denominator *= r.denominator;
			FixSign();
			Reduce();
			return *this;
		}

		rational& operator-= (const rational& r)
		{
			numerator = numerator * r.denominator - denominator * r.numerator;
			denominator *= r.denominator;
			FixSign();
			Reduce();
			return *this;
		}

		rational& operator*= (const rational& r)
		{
			numerator *= r.numerator;
			denominator *= r.denominator;
			Reduce();
			return *this;
		}

		rational& operator/= (const rational& r)
		{
			assert(r.numerator != 0);
			IntType n = r.numerator;
			numerator *= r.denominator;
			denominator *= n;
			FixSign();
			Reduce();
			return *this;
		}

		const rational& operator++()	// prefix increment
		{
			numerator += denominator;
			Reduce();
			return *this;
		}

		const rational operator++(int)	// postfix	increment
		{
			rational x(*this);
			numerator += denominator;
			Reduce();
			return x;
		}

		const rational& operator--()	// prefix decrement
		{
			numerator -= denominator;
			Reduce();
			return *this;
		}

		const rational operator--(int)	// postfix decrement
		{
			rational x = *this;
			numerator -= denominator;
			Reduce();
			return x;
		}

	private:
		void FixSign()	// insures that sign of the denominator is positive
		{
			if (denominator < IntType(0))
			{
				denominator = - denominator;
				numerator = -numerator;
			}
		}

		void Reduce()	// insures no common factors greater than 1
		{
			if (numerator == IntType(0))
			{
				denominator = IntType(1);
				return;
			}
			
			IntType n = numerator;
			
			if (n < IntType(0))
				n = -n;
			
			IntType d = GCD<IntType>(n, denominator);
			
			if (d > IntType(1))
			{
				numerator /= d;
				denominator /= d;
			}
		}

	public:
		long numerator, denominator;
	};

	// comparasons
	template<typename IntType> inline
	bool operator!= (const rational<IntType>& f, const rational<IntType>& g)
	{
		return !(f == g);
	}

	template<typename IntType> inline
	bool operator>= (const rational<IntType>& f, const rational<IntType>& g)
	{
		return (f > g) || (f == g);
	}

	template<typename IntType> inline
	bool operator< (const rational<IntType>& f, const rational<IntType>& g)
	{
		return (g > f);
	}

	template<typename IntType> inline
	bool operator<= (const rational<IntType>& f, const rational<IntType>& g)
	{
		return (f < g) || (f == g);
	}

	// arithmatic
	template<typename IntType> inline
	rational<IntType> operator+ (const rational<IntType>& f, const rational<IntType>& g)
	{
		rational<IntType> h = f;
		return h += g;
	}

	template<typename IntType> inline
	rational<IntType> operator- (const rational<IntType>& f, const rational<IntType>& g)
	{
		rational<IntType> h = f;
		return h -= g;
	}

	template<typename IntType> inline
	rational<IntType> operator* (const rational<IntType>& f, const rational<IntType>& g)
	{
		rational<IntType> h = f;
		return h *= g;
	}

	template<typename IntType> inline
	rational<IntType> operator/ (const rational<IntType>& f, const rational<IntType>& g)
	{
		rational<IntType> h = f;
		return h /= g;
	}

}
