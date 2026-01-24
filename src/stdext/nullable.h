#pragma once

namespace stdext
{
	template <class T>
	struct nullable
	{
		nullable() : is_null(true) {}
		nullable(T _value, bool _is_null = false) : value(_value), is_null(_is_null) {}
		nullable(const nullable& _nullable) : value(_nullable.value), is_null(_nullable.is_null) {}
		bool	is_null;
		T		value;
		nullable<T>& operator=(const T& _value) { is_null = false; value = _value; return *this; }
		nullable<T>& operator=(const nullable<T>& rhs) { is_null = rhs.is_null; value = rhs.value; return *this; }
	};

	template <class T>
	inline
	bool operator==(const nullable<T>& lhs, const nullable<T>& rhs)
	{
		return !(lhs.is_null || rhs.is_null) && (lhs.value == rhs.value);
	}

	template <class T>
	inline
	bool operator==(const nullable<T>& lhs, const T& rhs)
	{
		return !lhs.is_null && (lhs.value == rhs);
	}

	template <class T>
	inline
	bool operator!=(const nullable<T>& lhs, const nullable<T>& rhs)
	{
		return !(lhs.is_null || rhs.is_null) && (lhs.value != rhs.value);
	}

	template <class T>
	inline
	bool operator!=(const nullable<T>& lhs, const T& rhs)
	{
		return !lhs.is_null && (lhs.value != rhs);
	}

	template <class T>
	inline
	bool operator<(const nullable<T>& lhs, const nullable<T>& rhs)
	{
		return !(lhs.is_null || rhs.is_null) && (lhs.value < rhs.value);
	}

	template <class T>
	inline
	bool operator<(const nullable<T>& lhs, const T& rhs)
	{
		return !lhs.is_null && (lhs.value < rhs);
	}

	template <class T>
	inline
	bool operator<=(const nullable<T>& lhs, const nullable<T>& rhs)
	{
		return !(lhs.is_null || rhs.is_null) && (lhs.value <= rhs.value);
	}

	template <class T>
	inline
	bool operator<=(const nullable<T>& lhs, const T& rhs)
	{
		return !lhs.is_null && (lhs.value <= rhs);
	}

	template <class T>
	inline
	bool operator>(const nullable<T>& lhs, const nullable<T>& rhs)
	{
		return !(lhs.is_null || rhs.is_null) && (lhs.value > rhs.value);
	}

	template <class T>
	inline
	bool operator>(const nullable<T>& lhs, const T& rhs)
	{
		return !lhs.is_null && (lhs.value > rhs);
	}

	template <class T>
	inline
	bool operator>=(const nullable<T>& lhs, const nullable<T>& rhs)
	{
		return !(lhs.is_null || rhs.is_null) && (lhs.value >= rhs.value);
	}

	template <class T>
	inline
	bool operator>=(const nullable<T>& lhs, const T& rhs)
	{
		return !lhs.is_null && (lhs.value >= rhs);
	}
}