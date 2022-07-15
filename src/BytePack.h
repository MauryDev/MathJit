#pragma once
typedef unsigned char BYTE;

class BytePack
{
public:
	void Free();
	BytePack();
	BytePack(int size);
	BytePack(BYTE* buff,int size);
	BYTE* get_Address();
	template<typename T>
	T* get_AddressT();
	int get_Capacity();
	int get_CurrentIndex();
	void set_CurrentIndex(int index);
	void ResetIndex();
	void Write(BYTE* buff,int len);
	void Write(BYTE val);

	template<typename T>
	void WriteT(T val);
	template<typename T>
	void WriteTArray(T* val,int len);
	void WriteString(const char* val);
	void Read(BYTE* source, int len);
	BYTE Read();
	void ReadString(char* str, int maxlen);
	template<typename T>
	void ReadT(T* source);
	template<typename T>
	void ReadTArray(T** source,int len);
	void Resize(int newsize);
	BYTE* begin();
	BYTE* last();

private:
	BYTE* current;
	BYTE* buffer;
	int capacity;
};

template<typename T>
inline T * BytePack::get_AddressT()
{
	return (T*)get_Address();
}

template<typename T>
inline void BytePack::WriteT(T val)
{
	Write((BYTE*)&val, sizeof(T));
}

template<typename T>
inline void BytePack::WriteTArray(T * val, int len)
{
	for (size_t i = 0; i < len; i++)
	{
		WriteT(val[i]);
	}
}

template<typename T>
inline void BytePack::ReadT(T * source)
{
	Read((BYTE*)source, sizeof(T));
}

template<typename T>
inline void BytePack::ReadTArray(T ** source, int len)
{
	T* vals = *source;
	for (size_t i = 0; i < len; i++)
	{
		ReadT(&vals[i]);
	}
}
