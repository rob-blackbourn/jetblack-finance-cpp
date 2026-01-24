#pragma once

namespace stdext
{
	template<class T = double, bool _ByRow = true>
	class matrix2
	{
	public:
		typedef matrix2<T> _Myt;
		typedef T** iterator;
		typedef const T** const_iterator;

	public:
		matrix2()
		{	// construct empty matrix
			_Columns = _Rows = 0;
			_Data = 0;
		}

		explicit matrix2(size_t Columns, size_t Rows)
		{	// construct from bounds.
			_Columns = Columns;
			_Rows = Rows;

			_Data = new T*[_ByRow ? Rows : Columns];
			for (size_t j = 0; j < (_ByRow ? Rows : Columns); ++j)
			{
				_Data[j] = new T[_ByRow ? Columns : Rows];
			}
		}

		matrix2(const _Myt& _X)
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
		virtual ~matrix2()
		{
			_clear();
		}

		void resize(size_t Columns, size_t Rows)
		{
			T** Data = new T*[_ByRow ? Rows : Columns];
			for (size_t j = 0; j < (_ByRow ? Rows : Columns); ++j)
			{
				Data[j] = new T[_ByRow ? Columns : Rows];

				if (j < _Rows)
					memcpy(_Data[j], _M[j], sizeof(T) * ::min((_ByRow ? Columns : Rows), (_ByRow ? _Columns : _Rows)));
			}
			_clear();
			_Columns = Columns;
			_Rows = Rows;
			_Data = Data;
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
			return _Data + (_ByRow ? _Rows : _Columns);
		}

		const_iterator end() const
		{
			return _Data + (_ByRow ? _Rows : Columns);
		}

		const size_t Columns() const { return _Columns; }
		const size_t Rows() const { return _Columns; }

		size_t size() const
		{
			return _Columns * _Rows;
		}

		size_t max_size() const
		{
			return size();
		}

		bool empty() const
		{
			return _Columns == 0 || _Rows == 0;
		}

		T* operator[](size_t i)
		{
			return _Data[i];
		}

		const T* operator[](size_t i) const
		{
			return _Data[i];
		}

		bool operator==(const _Myt& _X)
		{
			if (! (_Columns == _X._Columns && _Rows == _X._Rows) )
				return false;
			for (size_t j = 0; j < (_ByRow ? _Rows : _Columns); ++j)
				for (size_t i = 0; i < (_ByRow ? _Columns : _Rows); ++i)
					if (_Data[j][i] != _X._Data[j][i])
						return false;
			return true;
		}

		bool operator<(const _Myt& _X)
		{
			if (_Rows >= _X._Rows)
				return false;
			if (_Columns >= _X._Columns)
				return false;
			for (size_t j = 0; j < (_ByRow ? _Rows : _Columns); ++j)
				for (size_t i = 0; i < (_ByRow ? _Columns : _Rows); ++i)
					if (_Data[j][i] >= _X._Data[j][i])
						return false;
			return true;
		}

	private:

		void _copy(const _Myt& _X)
		{
			_Columns = Columns;
			_Rows = Rows;

			if (_Columns && _Rows)
			{
				_Data = new T*[_ByRow ? _Rows : _Columns];
				for (size_t j = 0; j < (_ByRow ? _Rows : _Columns); ++j)
				{
					_Data[j] = new T[_ByRow ? _Columns : _Rows];
					memcpy(_Data[j], _X._Data[j], sizeof(T) * (_ByRow ? _Columns : _Rows));
				}
			}
			else
				_Data = 0;
		}

		void _clear()
		{
			if (_Data)
			{
				for (size_t j = 0; j < (_ByRow ? _Rows : _Columns); ++j)
					delete [] _Data[j];
				delete [] _Data;
				_Data = 0;
			}
			_Columns = _Rows = 0;
		}

	protected:
		size_t _Columns, _Rows;
		T** _Data;
	};
}
