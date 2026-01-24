#pragma once

#include <stdexcept>
#include <cstdarg>

namespace stdext
{
	/*
	 * Given the bounds of an array and the nD indices, returns a 1D index
	 */
	template<size_t _N>
	inline
	size_t from_dims(const size_t bounds[], const size_t dims[])
	{
		size_t rev_bounds[_N], rev_dims[_N];
		for (size_t i = 0; i < _N; ++i)
		{
			rev_bounds[_N - (i+1)] = bounds[i];
			rev_dims[_N - (i+1)] = dims[i];
		}

		size_t n = 1;
		for (size_t i = 0; i < _N; ++i)
			n = n * rev_bounds[i];

		size_t cell = rev_dims[0];
		if (cell < 0 || cell >= n)
			throw "Initial index out of bounds";

		size_t sum = 1;
		for (size_t dim = 1; dim < _N; ++dim)
		{
			if (rev_bounds[dim] == 0 || rev_dims[dim] < 0 || rev_dims[dim] >= rev_bounds[dim])
				throw "Index out of bounds";
			sum *= rev_bounds[dim];
			cell += rev_dims[dim] * sum;
		}

		return cell;
	}

	template<>
	inline
	size_t from_dims<0>(const size_t bounds[], const size_t dims[])
	{
		throw "there are no dimensions";
	}

	template<>
	inline
	size_t from_dims<1>(const size_t bounds[], const size_t dims[])
	{
		return dims[0];
	}

	template<>
	inline
	size_t from_dims<2>(const size_t bounds[], const size_t dims[])
	{
		return
			dims[0] * bounds[1]
			+ dims[1];
	}

	template<>
	inline
	size_t from_dims<3>(const size_t bounds[], const size_t dims[])
	{
		return
			dims[0] * bounds[1] * bounds[2]
			+ dims[1] * bounds[2]
			+ dims[2];
	}

	template<>
	inline
	size_t from_dims<4>(const size_t bounds[], const size_t dims[])
	{
		return
			dims[0] * bounds[1] * bounds[2] * bounds[3]
			+ dims[1] * bounds[2] * bounds[3]
			+ dims[2] * bounds[3]
			+ dims[3];
	}

	/*
	 * Given the bounds of an array and a 1D index, returns the nD indices
	 */
	template<size_t _N>
	inline
	size_t* to_dims(const size_t bounds[], size_t index, size_t dims[])
	{
		size_t rev_bounds[_N], rev_dims[_N];
		for (size_t i = 0; i < _N; ++i)
			rev_bounds[_N - (i+1)] = bounds[i];
	    
		rev_dims[0] = index % rev_bounds[0];
	    
		size_t sum = rev_bounds[0];
		for (size_t dim = 1; dim < _N; ++dim)
		{
			rev_dims[dim] = (index / sum) % rev_bounds[dim];
			sum *= rev_bounds[dim];
		}

		for (size_t i = 0; i < _N; ++i)
			dims[_N - (i+1)] = rev_dims[i];

		return dims;
	}

	template<>
	inline
	size_t* to_dims<1>(const size_t bounds[], size_t index, size_t dims[])
	{
		dims[0] = index;
		return dims;
	}

	template<>
	inline
	size_t* to_dims<2>(const size_t bounds[], size_t index, size_t dims[])
	{
		dims[1] = index % bounds[1];
		dims[0] = index / bounds[1];
		return dims;
	}

	template<>
	inline
	size_t* to_dims<3>(const size_t bounds[], size_t index, size_t dims[])
	{
		dims[2] = index % bounds[2];
		dims[1] = (index / bounds[2]) % bounds[1];
		dims[0] = (index / (bounds[2] * bounds[1])) % bounds[0];
		return dims;
	}

	template<>
	inline
	size_t* to_dims<4>(const size_t bounds[], size_t index, size_t dims[])
	{
		dims[3] = index % bounds[3];
		dims[2] = (index / bounds[3]) % bounds[2];
		dims[1] = (index / (bounds[2] * bounds[3])) % bounds[1];
		dims[0] = (index / (bounds[1] * bounds[2] * bounds[3])) % bounds[0];
		return dims;
	}

	template<size_t _N = 2, class _Ty = double>
	class matrix
	{
	public:
		typedef matrix<_N, _Ty> _Myt;
		typedef _Ty value_type;
		typedef _Ty* pointer;
		typedef const _Ty* const_pointer;
		typedef typename _Ty& reference;
		typedef typename const _Ty& const_reference;
		typedef _Ty* iterator;
		typedef const _Ty* const_iterator;

	public:
		matrix()
		{	// construct empty matrix
			memset(_Bounds, 0, _N * sizeof(_N));
			_Size = 0;
			_Data = 0;
		}

		explicit matrix(size_t _B[_N], const _Ty& _M = _Ty())
		{	// construct from bounds.
			memcpy(_Bounds, _B, _N * sizeof(_N));
			_Size = _product(_Bounds);
			_Data = new _Ty[_Size];
			for (size_t i = 0; i < _Size; ++i)
				_Data[i] = _M;
		}

		matrix(size_t dim1, ...)
		{	// varargs constructor
			va_list marker;
			va_start(marker, dim1);
			_Bounds[0] = dim1;
			for (size_t i = 1; i < _N; ++i)
				_Bounds[i] = va_arg(marker, size_t);
			va_end(marker);

			_Size = _product(_Bounds);
			_Data = new _Ty[_Size];
			for (size_t i = 0; i < _Size; ++i)
				_Data[i] = _Ty();
		}

		matrix(const _Myt& _X)
		{	// copy constructor
			_copy(_X);
		}

		_Myt operator=(const _Myt& _X)
		{
			if (this != &_X)
			{
				_clear();
				_copy(_X);
			}
			return *this;
		}

		// We can let the parent class cleanup for us.
		virtual ~matrix()
		{
			_clear();
		}

		void resize(size_t _B[_N])
		{
			_clear();
			memcpy(_Bounds, _B, _N * sizeof(_N));
			_Size = _product(_Bounds);
			_Data = new _Ty[_Size];
			for (size_t i = 0; i < _Size; ++i)
				_Data[i] = _Ty();
		}

		iterator begin()
		{
			return _Data;
		}

		const_iterator begin() const
		{
			return _Data;
		}

		iterator end()
		{
			return _Data + _Size;
		}

		const_iterator end() const
		{
			return _Data + _Size;
		}

		const size_t* bounds() const { return _Bounds; }

		size_t dims() const { return _N; }

		size_t size() const
		{
			return _Size;
		}

		size_t max_size() const
		{
			return _Size;
		}

		bool empty() const
		{
			return _Size == 0;
		}

		value_type operator() (size_t dims[_N]) const
		{
			return _Data[from_index(dims)];
		}

		value_type operator()(size_t dim1, ...) const
		{
			size_t dims[_N];
			va_list marker;

			va_start(marker, dim1);
			dims[0] = dim1;
			for (size_t i = 1; i < _N; ++i)
				dims[i] = va_arg(marker, size_t);
			va_end(marker);
			return _Data[from_index(dims)];
		}

		const_reference at(size_t dims[_N]) const
		{
			size_t i = from_index(dims);
			if (i >= _Size)
				throw std::out_of_range("invalid matrix<N,T> subscript");
			return _Data[i];
		}

		const_reference at(size_t dim1, ...) const
		{
			size_t dims[_N];
			va_list marker;

			va_start(marker, dim1);
			dims[0] = dim1;
			for (size_t i = 1; i < _N; ++i)
				dims[i] = va_arg(marker, size_t);
			va_end(marker);
			size_t i = from_index(dims); 
			if (i >= _Size)
				throw std::out_of_range("invalid matrix<N,T> subscript");
			return _Data[i];
		}

		reference operator()(size_t dims[_N])
		{
			return _Data[from_index(dims)];
		}

		reference operator()(size_t dim1, ...)
		{
			size_t dims[_N];
			va_list marker;

			va_start(marker, dim1);
			dims[0] = dim1;
			for (size_t i = 1; i < _N; ++i)
				dims[i] = va_arg(marker, size_t);
			va_end(marker);
			return _Data[from_index(dims)];
		}

		reference at(size_t dims[_N])
		{
			size_t i = from_index(dims);
			if (i >= _Size)
				throw std::out_of_range("invalid matrix<N,T> subscript");
			return _Data[i];
		}

		reference at(size_t dim1, ...)
		{
			size_t dims[_N];
			va_list marker;

			va_start(marker, dim1);
			dims[0] = dim1;
			for (size_t i = 1; i < _N; ++i)
				dims[i] = va_arg(marker, size_t);
			va_end(marker);
			size_t i = from_index(dims); 
			if (i >= _Size)
				throw std::out_of_range("invalid matrix<N,T> subscript");
			return _Data[i];
		}

		reference operator[](size_t i)
		{
			return _Data[i];
		}

		value_type operator[](size_t i) const
		{
			return _Data[i];
		}

		bool operator==(const _Myt& _X)
		{
			if (_Size != _X._Size)
				return false;
			for (size_t i = 0; i < _N; ++i)
				if (_Bounds[i] != _X._Bounds[i])
					return false;
			for (size_t i = 0; i < _Size; ++i)
				if (_Data[i] != _X._Data[i])
					return false;
			return true;
		}

		bool operator<(const _Myt& _X)
		{
			if (_Size >= _X._Size)
				return false;
			for (size_t i = 0; i < _N; ++i)
				if (_Bounds[i] >= _X._Bounds[i])
					return false;
			for (size_t i = 0; i < _Size; ++i)
				if (_Data[i] >= _X._Data[i])
					return false;
			return true;
		}

		size_t from_index(const size_t dims[]) const
		{
			return from_dims<_N>((const size_t*) _Bounds, dims);
		}
		

		size_t* to_index(size_t index, size_t dims[_N]) const
		{
			return to_dims<_N>((const size_t*) _Bounds, index, dims);
		}


	private:
		size_t _product(size_t _B[_N])
		{
			size_t sum = 1;
			for (size_t* p = _B; p < _B+_N; ++p)
				sum *= *p;
			return sum;
		}

		void _copy(const _Myt& _X)
		{
			memcpy(_Bounds, _X._Bounds, _N * sizeof(_N));
			_Size = _X._Size;
			if (_Size)
			{
				_Data = new _Ty[_Size];
				for (size_t i = 0; i < _Size; ++i)
					_Data[i] = _X._Data[i];
			}
			else
				_Data = 0;
		}

		void _clear()
		{
			if (_Data)
			{
				delete [] _Data;
				_Data = 0;
			}
			memset(_Bounds, 0, _N * sizeof(_N));
			_Size = 0;
		}

	protected:
		size_t _Bounds[_N];
		size_t _Size;
		pointer _Data;
	};

}
