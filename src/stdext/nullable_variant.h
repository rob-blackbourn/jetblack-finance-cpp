#pragma once

inline double& operator<<(double& lhs, const VARIANT& rhs) throw (std::runtime_error)
{
	switch (rhs.vt)
	{
	case VT_R8:
		lhs = rhs.dblVal;
		break;
	case VT_R4:
		lhs = static_cast<double>(rhs.fltVal);
		break;
	case VT_I2:
		lhs = static_cast<double>(rhs.intVal);
		break;
	case VT_I4:
		lhs = static_cast<double>(rhs.lVal);
		break;
	default:
		throw std::runtime_error("type mismatch");
	}

	return lhs;
}

inline char& operator<<(char& lhs, const VARIANT& rhs) throw (std::runtime_error)
{
	switch (rhs.vt)
	{
	case VT_I1:
		lhs = static_cast<char>(rhs.bVal);
		break;
	case VT_BSTR:
		lhs = static_cast<char>(rhs.bstrVal[0]);
		break;
	default:
		throw std::runtime_error("type mismatch");
	}

	return lhs;
}

inline unsigned char& operator<<(unsigned char& lhs, const VARIANT& rhs) throw (std::runtime_error)
{
	switch (rhs.vt)
	{
	case VT_UI1:
		lhs = static_cast<int>(rhs.bVal);
		break;
	default:
		throw std::runtime_error("type mismatch");
	}

	return lhs;
}

inline short& operator<<(short& lhs, const VARIANT& rhs) throw (std::runtime_error)
{
	switch (rhs.vt)
	{
	case VT_I2:
		lhs = static_cast<short>(rhs.iVal);
		break;
	default:
		throw std::runtime_error("type mismatch");
	}

	return lhs;
}

inline int& operator<<(int& lhs, const VARIANT& rhs) throw (std::runtime_error)
{
	switch (rhs.vt)
	{
	case VT_I2:
		lhs = static_cast<int>(rhs.iVal);
		break;
	case VT_I4:
		lhs = static_cast<int>(rhs.lVal);
		break;
	default:
		throw std::runtime_error("type mismatch");
	}

	return lhs;
}

inline std::string& operator<<(std::string& lhs, const VARIANT& rhs) throw (std::runtime_error)
{
	switch (rhs.vt)
	{
	case VT_BSTR:
		lhs = (const char*) _bstr_t(rhs.bstrVal);
		break;
	default:
		throw std::runtime_error("type mismatch");
	}

	return lhs;
}

inline COleDateTime& operator<<(COleDateTime& lhs, const VARIANT& rhs) throw (std::runtime_error)
{
	switch (rhs.vt)
	{
	case VT_DATE:
		lhs = rhs.date;
		break;
	default:
		throw std::runtime_error("type mismatch");
	}

	return lhs;
}

inline bool& operator<<(bool& lhs, const VARIANT& rhs) throw (std::runtime_error)
{
	switch (rhs.vt)
	{
	case VT_BOOL:
		lhs = rhs.boolVal == VARIANT_TRUE;
		break;
	default:
		throw std::runtime_error("type mismatch");
	}

	return lhs;
}

template <class T> inline stdext::nullable<T>& operator << (stdext::nullable<T>& x, const VARIANT& rhs) throw (std::runtime_error)
{
	if (rhs.vt == VT_NULL)
		x.is_null = true;
	else
	{
		x.is_null = false;
		x.value << rhs;
	}

	return x;
}
