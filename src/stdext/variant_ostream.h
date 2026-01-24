#pragma once

inline
std::ostream& operator<<(std::ostream& os, const CY& cy)
{
	return os << cy.int64 / 10000 << "." << cy.int64 % 10000;
}

inline
std::ostream& operator <<(std::ostream& os, const _variant_t& v)
{
	switch (v.vt)
	{
	case VT_EMPTY:
		return os << "#EMPTY#";
	case VT_NULL:
		return os << "#NULL#";
	case VT_I2:
		return os << static_cast<int>(v.iVal);
	case VT_I2|VT_BYREF:
		return os << static_cast<int>(*(v.piVal));
	case VT_I4:
		return os << v.lVal;
	case VT_I4|VT_BYREF:
		return os << *(v.plVal);
	case VT_R4:
		return os << v.fltVal;
	case VT_R4|VT_BYREF:
		return os << *(v.pfltVal);
	case VT_R8:
		return os << v.dblVal;
	case VT_R8|VT_BYREF:
		return os << *(v.pdblVal);
	case VT_CY:
		return os << v.cyVal;
	case VT_CY|VT_BYREF:
		return os << *(v.pcyVal);
	case VT_DATE:
		return os << COleDateTime(v.date);
	case VT_DATE|VT_BYREF:
		return os << COleDateTime(*(v.pdate));
	case VT_BSTR:
		return os << (const char*) _bstr_t(v.bstrVal);
	case VT_BSTR|VT_BYREF:
		return os << (const char*) _bstr_t(*(v.pbstrVal));
	case VT_DISPATCH:
	case VT_DISPATCH|VT_BYREF:
		return os << "#DISPATCH#";
	case VT_ERROR:
		return os << "#ERROR#";
	case VT_ERROR|VT_BYREF:
		return os << *(v.pscode);
	case VT_BOOL:
		return os << (v.boolVal == VARIANT_TRUE);
	case VT_BOOL|VT_BYREF:
		return os << (*(v.pboolVal) == VARIANT_TRUE);
	case VT_VARIANT|VT_BYREF:
		return os << _variant_t(*((VARIANT*) v.byref), false);
	case VT_I1:
		return os << v.cVal;
	case VT_I1|VT_BYREF:
		return os << *(v.pcVal);
	case VT_UI1:
		return os << v.bVal;
	case VT_UI1|VT_BYREF:
		return os << *(v.pbVal);
	case VT_UI2:
		return os << v.uiVal;
	case VT_UI2|VT_BYREF:
		return os << *(v.puiVal);
	case VT_UI4:
		return os << v.ulVal;
	case VT_UI4|VT_BYREF:
		return os << (*v.pulVal);
	case VT_I8:
		return os << v.llVal;
	case VT_I8|VT_BYREF:
		return os << *(v.pllVal);
	case VT_UI8:
		return os << v.ullVal;
	case VT_UI8|VT_BYREF:
		return os << *(v.pullVal);
	case VT_INT:
		return os << v.intVal;
	case VT_INT|VT_BYREF:
		return os << *(v.pintVal);
	case VT_UINT:
		return os << v.uintVal;
	case VT_UINT|VT_BYREF:
		return os << *(v.puintVal);
	case VT_ARRAY|VT_VARIANT:
		{
			size_t nelems = 1;
			for (size_t i = 0; i < v.parray->cDims; ++i)
				nelems *= v.parray->rgsabound[i].cElements;
			VARIANT* pData;
			HRESULT hr = SafeArrayAccessData(v.parray, (void**) &pData);
			if (FAILED(hr)) return os;
			for (size_t i = 0; i < nelems; ++i)
			{
				if (i > 0) os << '|';
				os << _variant_t(pData[i]);
			}
			SafeArrayUnaccessData(v.parray);
		}
	default:
		return os << "#UNHANDLED#";
	}
}
