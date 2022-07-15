#include "BytePack.h"
#include <Windows.h>
void BytePack::Free()
{
	if (buffer)
	{
		free(this->buffer);
		capacity = 0;
		current = 0;
		buffer = NULL;
	}
}
BytePack::BytePack()
{
	buffer = nullptr;
	current = buffer;
	capacity = 0;
}

BytePack::BytePack(int size)
{
	// ou malloc
	buffer = (BYTE*)malloc(size);
	current = buffer;
	capacity = size;
}

BytePack::BytePack(BYTE * buff, int size)
{
	current = buff;
	buffer = buff;
	capacity = size;
}

BYTE * BytePack::get_Address()
{
	return current;
}

int BytePack::get_Capacity()
{
	return capacity;
}

int BytePack::get_CurrentIndex()
{
	return current - buffer;
}

void BytePack::set_CurrentIndex(int index)
{
	if (index < capacity)
	{
		current = buffer + index;
	}
}

void BytePack::ResetIndex()
{
	set_CurrentIndex(0);
}

void BytePack::Write(BYTE * buff, int len)
{
	for (size_t i = 0; i < len; i++)
	{
		Write(buff[i]);
	}
}

void BytePack::Write(BYTE val)
{
	auto index = get_CurrentIndex();
	if (index >= capacity)
	{
		Resize(capacity == 0 ? 16 : capacity * 2);
	}
	*(current++) = val;

}


void BytePack::WriteString(const char * val)
{
	auto lenstr = strlen(val);
	Write((BYTE*)val, lenstr);
	Write(0);
}

void BytePack::Read(BYTE * source, int len)
{
	auto index = get_CurrentIndex();
	if (index + len < capacity)
	{
		for (size_t i = 0; i < len; i++)
		{
			source[i] = Read();
		}
	}
}

BYTE BytePack::Read()
{
	auto index = get_CurrentIndex();
	if (index < capacity)
		return *(current++);
	return 0;
}

void BytePack::ReadString(char * str, int maxlen)
{
	auto val = Read();
	while (val != 0 && maxlen > 1)
	{
		*(str++) = val;
		maxlen--;
		val = Read();
	}
	if (maxlen > 0)
		*(str++) = 0;
}

void BytePack::Resize(int newsize)
{
	if (newsize == capacity)
	{
		return;
	}
	auto index = get_CurrentIndex();
	
	auto copysize = min(capacity, newsize);
	auto newbuffer = (BYTE*)malloc(newsize);
	for (size_t i = 0; i < copysize; i++)
	{
		newbuffer[i] = buffer[i];
	}
	free(buffer);
	capacity = newsize;

	buffer = newbuffer;
	if (newsize == copysize)
	{
		set_CurrentIndex(newsize - 1);
	}
	else
	{
		set_CurrentIndex(index);

	}
}

BYTE * BytePack::begin()
{
	return buffer;
}

BYTE * BytePack::last()
{
	return current;
}
