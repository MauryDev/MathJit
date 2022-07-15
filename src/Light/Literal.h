#pragma once
#include <cstring>
#include <initializer_list>
#include <malloc.h>

namespace Light
{
	template<typename T>
	inline T* TAlloc(int size)
	{
		return (T*)malloc(sizeof(T)*size);
	}
	class string_view
	{
	public:
		string_view();
		string_view(const char* str);
		string_view(const char* str, int len);
		char* data();
		size_t find(char c, size_t offset);
		bool startWith(const char* str, size_t offset);
		size_t size();
		char& operator [](int index);
		char& at(int index);
		string_view SubString(int index);
		bool Equals(const char* str);
		bool Equals(const string_view& str);
		bool Equals(const string_view& str) const;

		bool operator ==(const string_view& str) const;
		int Hash() const;
		string_view Clone();
		string_view operator =(const char* strng);

		void Free();
	private:
		char* str;
		size_t len;
	};
	struct StringView_Hash
	{
		std::size_t operator()(const string_view& k) const;
	};
	class StringBuffer
	{
	public:
		StringBuffer();
		void Add(char c);
		void RemoveLast();
		void resize(int newsize);
		string_view ToString();
	private:
		int offset;
		int len;
		char* buff;
	};
	template<typename T>
	class Array_Temp
	{
	public:
		Array_Temp(T* arr);
		~Array_Temp();
		void Free();
	private:
		T* buff;
	};
	template<typename T>
	class Array_Enumerator;
	template<typename T>
	class Array
	{
	public:
		static const int default_capacity = 4;
		Array(int capacity,T* buffer);
		Array();
		Array(int capacity);
		void Add(T c);
		void AddRange(std::initializer_list<T> c);
		void RemoveLast();
		void RemoveAt(int index);
		void RemoveAt(int index, int len);

		int get_Count();
		int get_Capacity();
		void Resize(int newsize);
		T& operator [](int index);
		T& at(int index);
		bool Contains(T value);
		void Clear();
		T* begin();
		T* end();
		T& last();
		void Free();
		static Light::Array<T> Empty();
		Array_Enumerator<T> get_Enumerator();
		Array_Temp<T> ToTemp();
	private:
		int offset;
		int len;
		T* buff;
	};
	template<typename T>
	class Array_Enumerator
	{
	public:
		Array_Enumerator(T* buff, T* end);
		bool Next();
		T& get_Current();
	private:
		T* current;
		T* offset;
		T* end;
	};
	template<typename T>
	inline Array<T>::Array(int capacity, T * buffer)
	{
		offset = 0;
		len = capacity;
		buff = buffer;
	}
	template<typename T>
	inline Array<T>::Array()
	{
		offset = 0;
		len = 0;
		buff = NULL;
	}

	template<typename T>
	inline Array<T>::Array(int capacity)
	{
		offset = 0;
		len = capacity;
		buff = TAlloc<T>(capacity);
	}

	template<typename T>
	inline void Array<T>::Add(T c)
	{
		if (offset >= len)
		{
			Resize((len == 0) ? default_capacity : len * 2);
		}
		auto f = offset++;
		buff[f] = c;
	}

	template<typename T>
	inline void Array<T>::AddRange(std::initializer_list<T> c)
	{
		for (T v : c)
		{
			Add(v);
		}
	}

	template<typename T>
	inline void Array<T>::RemoveLast()
	{
		if (offset > 0)
		{
			--offset;
		}
	}

	template<typename T>
	inline void Array<T>::RemoveAt(int index)
	{
		if (index < offset)
		{
			auto deleted = &buff[index];
			auto next = deleted + 1;
			auto end_ = end();
			while (next != end_)
			{
				*(deleted++) = *(next++);
			}
			offset--;
		}
	}

	template<typename T>
	inline void Array<T>::RemoveAt(int index, int len)
	{
		if (index + len - 1 < offset)
		{
			auto deleted = &buff[index];
			auto next = deleted + len;
			auto end_ = end();
			while (next != end_)
			{
				*(deleted++) = *(next++);
			}
			offset -= len;
		}
	}

	template<typename T>
	inline int Array<T>::get_Count()
	{
		return offset;
	}

	template<typename T>
	inline int Array<T>::get_Capacity()
	{
		return len;
	}

	template<typename T>
	inline void Array<T>::Resize(int newsize)
	{
		auto newbuff = TAlloc<T>(newsize);
		auto minlen = len < newsize ? len : newsize;
		for (size_t i = 0; i < minlen; i++)
		{
			newbuff[i] = buff[i];
		}
		delete[] buff;
		buff = newbuff;
		len = newsize;
		if (offset >= newsize)
		{
			offset = newsize;
		}
	}

	template<typename T>
	inline T & Array<T>::operator[](int index)
	{
		return buff[index];
	}

	template<typename T>
	inline T & Array<T>::at(int index)
	{
		return buff[index];
	}

	template<typename T>
	inline bool Array<T>::Contains(T value)
	{
		T* end = buff + len;
		T* temp = buff;
		while (temp != end)
		{
			if (*(temp++) == value)
				return true;
		}
		return false;
	}

	template<typename T>
	inline void Array<T>::Clear()
	{
		offset = 0;
	}

	template<typename T>
	inline T* Array<T>::begin()
	{
		return buff;
	}

	template<typename T>
	inline T * Array<T>::end()
	{
		return buff + offset;
	}

	template<typename T>
	inline T & Array<T>::last()
	{
		return *(buff + offset - 1);
	}

	template<typename T>
	inline void Array<T>::Free()
	{
		if (buff != NULL)
		{
			delete[] buff;
			this->buff = NULL;
			this->len = 0;
			this->offset = NULL;
		}
		
	}

	template<typename T>
	inline Light::Array<T> Array<T>::Empty()
	{
		return Light::Array<T>(0,NULL);
	}

	template<typename T>
	inline Array_Enumerator<T> Array<T>::get_Enumerator()
	{
		return Array_Enumerator<T>(begin(), end());
	}

	template<typename T>
	inline Array_Temp<T> Array<T>::ToTemp()
	{
		return Array_Temp<T>(buff);
	}

	template<typename T>
	inline Array_Enumerator<T>::Array_Enumerator(T * buff, T * end)
	{
		offset = buff;
		this->end = end;
	}

	template<typename T>
	inline bool Array_Enumerator<T>::Next()
	{
		auto flag = end != offset;
		if (flag)
		{
			current = offset++;
		}
		return flag;
	}

	template<typename T>
	inline T & Array_Enumerator<T>::get_Current()
	{
		return *current;
	}

	template<typename T>
	inline Array_Temp<T>::Array_Temp(T * arr)
	{
		buff = arr;
	}

	template<typename T>
	inline Array_Temp<T>::~Array_Temp()
	{
		if (buff != NULL)
		{
			delete[] buff;
			buff = NULL;
		}
	}

	template<typename T>
	inline void Array_Temp<T>::Free()
	{
		if (buff != NULL)
		{
			delete[] buff;
			buff = NULL;
		}
	}

}