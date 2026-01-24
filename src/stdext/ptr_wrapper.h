#pragma once

namespace stdext
{
	template<typename T> class ptr_wrapper
	{
	private:
		T* _ptr;

	public:
		ptr_wrapper() : _ptr(0) {}
		ptr_wrapper(T* ptr) : _ptr(ptr) {}
		ptr_wrapper(const ptr_wrapper& x) : _ptr(x._ptr) {}

	public:
		T* operator = (T* ptr) { _ptr = ptr; return _ptr; }
		operator T*() { return _ptr; }
		operator const T*() const { return _ptr; }
		T* operator -> () { return _ptr; }
		const T* operator -> () const { return _ptr; }
	};
}
