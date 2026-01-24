#pragma once

namespace stdext
{
	template <typename T>
	inline std::vector<std::basic_string<T> > split(const std::basic_string<T>& s, const T& c)
	{
		std::vector<std::basic_string<T> > vec;
		std::basic_string<T>::const_iterator start = s.begin();
		std::basic_string<T>::const_iterator next = s.begin();
		while (next != s.end())
		{
			if (*next == c)
			{
				std::basic_string<T> s1(start, next);
				vec.push_back(s1);
				start = next + 1;
			}
			++next;
		}
		if (start != next || *(next-1) == c)
		{
			std::basic_string<T> s1(start, next);
			vec.push_back(s1);
		}

		return vec;
	}

	template <typename T>
	inline
	std::basic_string<T>
	join(
		typename std::vector<std::basic_string<T> >::const_iterator& begin,
		typename std::vector<std::basic_string<T> >::const_iterator& end,
		const T&c)
	{
		std::basic_string<T> s;

		for (std::vector<std::basic_string<T> >::const_iterator i(begin); i != end; ++i)
		{
			if (!s.empty())
				s += c;
			s += *i;
		}
		return s;
	}

	inline
	char* ltrim(char* s)
	{
		char *s1 = s;
		while (isspace(*s1)) ++s1;
		return (char*) memmove((void*) s, (void*) s1, 1 + strlen(s1));
	}

	inline
	char* rtrim(char* s)
	{
		char *s1 = s;
		while (*s1 != '\0')
			++s1;
		--s1;
		while (s1 >= s && isspace(*s1))
			*s1-- = '\0';
		return s;
	}

	inline
	char* trim(char* s)
	{
		return ltrim(rtrim(s));
	}

	inline
	char* compressws(char* dest, const char* src)
	{
		char *s = dest;
		bool was_ws = false;
		while (*src)
		{
			bool is_ws = (isspace(*src) != 0);

			if (!is_ws || (is_ws && !was_ws))
				*dest++ = *src;

			was_ws = is_ws;

			++src;
		}
		*dest = *src;

		return s;
	}

	inline double atod(const char* s)
	{
		static char *month_names[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
		int day, mon, year;
		char month[4];
		sscanf(s, "%d-%3s-%d", &day, month, &year);
		for (mon = 0; mon < 12; ++mon)
			if (stricmp(month, month_names[mon]) == 0)
				break;
		return (double) COleDateTime(year, mon+1, day, 0, 0, 0);
	}

	inline std::string dtoa(const COleDateTime& date)
	{
		static char *month_names[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
		std::stringstream s;
		s << date.GetDay() << "-" << month_names[date.GetMonth() - 1] << "-" << date.GetYear();
		return s.str();
	}

	struct ToUpper
	{
		ToUpper(std::locale const& l) : loc(l) {;}
		char operator() (char c) const { return std::toupper(c, loc); }
	private:
		std::locale const& loc;
	};

	inline std::string uppercase(const std::string& src, std::locale const& l = std::locale())
	{
		std::string dest;
		dest.resize(src.size());
		std::transform(src.begin(), src.end(), dest.begin(), ToUpper(l));
		return dest;
	}
}