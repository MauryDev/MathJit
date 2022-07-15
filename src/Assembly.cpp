#include "Assembly.h"
#include <Windows.h>

Assembly::Assembly()
{

}

Assembly::Assembly(int size, BYTE * buff)
{
	this->bp = BytePack(buff, size);
}

void Assembly::MovPTRESP(int val)
{
	BYTE bytes[] = { 0xC7,4,0x24 };
	bp.Write(bytes, 3);
	bp.WriteT(val);
}

void Assembly::Nop()
{
	bp.Write(0x90);
}

void Assembly::PushEbp()
{
	bp.Write(0x55);
}

void Assembly::MovEbpEsp()
{
	BYTE bytes[2] = { 0x89,0xE5 };
	bp.Write(bytes,2);
}
void Assembly::fmulp_st1_st()
{
	BYTE bytes[2] = { 0xDE, 0xC9 };
	bp.Write(bytes,2);

}
void Assembly::faddp_st1_st()
{
	BYTE bytes[2] = { 0xDE, 0xC1 };
	bp.Write(bytes, 2);
}
void Assembly::fsubp_st1_st()
{
	BYTE bytes[2] = { 0xDE, 0xE9 };
	bp.Write(bytes, 2);
}
void Assembly::fdivp_st1_st()
{
	BYTE bytes[2] = { 0xDE, 0xF9 };
	bp.Write(bytes, 2);
}
void Assembly::fldz()
{
	BYTE bytes[2] = { 0xD9, 0xEE };
	bp.Write(bytes, 2);
}
void Assembly::fld1()
{
	BYTE bytes[2] = { 0xD9, 0xE8 };
	bp.Write(bytes, 2);
}
void Assembly::fldpi()
{
	BYTE bytes[2] = { 0xD9 , 0xEB };
	bp.Write(bytes, 2);
}
void Assembly::fldl2e()
{
	BYTE bytes[2] = { 0xD9 , 0xEA };
	bp.Write(bytes, 2);
}
void Assembly::fldl2t()
{
	BYTE bytes[2] = { 0xD9 , 0xE9 };
	bp.Write(bytes, 2);
}
void Assembly::fldlg2()
{
	BYTE bytes[2] = { 0xD9 , 0xEC };
	bp.Write(bytes, 2);
}
void Assembly::fldln2()
{
	BYTE bytes[2] = { 0xD9 , 0xED };
	bp.Write(bytes, 2);
}
void Assembly::fsqrt()
{
	BYTE bytes[2] = { 0xD9 , 0xFA };
	bp.Write(bytes, 2);
}
void Assembly::fabs()
{
	BYTE bytes[2] = { 0xD9 , 0xE1 };
	bp.Write(bytes, 2);
}
void Assembly::frndint()
{
	BYTE bytes[2] = { 0xD9 , 0xFC };
	bp.Write(bytes, 2);
}
void Assembly::fcos()
{
	BYTE bytes[2] = { 0xD9 , 0xFF };
	bp.Write(bytes, 2);
}
void Assembly::fsin()
{
	BYTE bytes[2] = { 0xD9 , 0xFE };
	bp.Write(bytes, 2);
}
void Assembly::fsincos()
{
	BYTE bytes[2] = { 0xD9 , 0xFB };
	bp.Write(bytes, 2);
}
void Assembly::fptan()
{
	BYTE bytes[2] = { 0xD9 , 0xF2 };
	bp.Write(bytes, 2);
}
void Assembly::fpatan()
{
	BYTE bytes[2] = { 0xD9 , 0XF3 };
	bp.Write(bytes, 2);
}
void Assembly::fchs()
{
	BYTE bytes[2] = { 0xD9 , 0XE0 };
	bp.Write(bytes, 2);
}
void Assembly::movEspEbp()
{
	BYTE bytes[2] = { 0x89, 0xEC };
	bp.Write(bytes, 2);
}
void Assembly::movEaxEBP(BYTE offset)
{
	BYTE bytes[2] = { 0x8B, 0x45 };
	bp.Write(bytes, 2);
	bp.Write(offset);
}
void Assembly::CmpEbpEax(BYTE offset)
{
	BYTE bytes[2] = { 0x39, 0x45 };
	bp.Write(bytes, 2);
	bp.Write(offset);
}
void Assembly::_SubEsp(int value)
{
	if (value > 127 || value < -128)
	{
		SubEsp(value);
	}
	else
	{
		SubEsp((BYTE)value);
	}
}
void Assembly::AddEsp(BYTE value)
{
	BYTE bytes[2] = { 0x83, 0xC4 };
	bp.Write(bytes, 2);
	bp.Write(value);
}
void Assembly::AddEsp(int value)
{
	BYTE bytes[2] = { 0x81, 0xC4 };
	bp.Write(bytes, 2);
	bp.WriteT(value);
}
void Assembly::LeaEaxEbp(BYTE value)
{
	BYTE bytes[2] = { 0x8D, 0x45 };
	bp.Write(bytes, 2);
	bp.Write(value);
}
void Assembly::MovEBP(BYTE offset, int value)
{
	BYTE bytes[2] = { 0xC7, 0x45 };
	bp.Write(bytes, 2);
	bp.Write(offset);
	bp.WriteT(value);
}
void Assembly::SubEsp(BYTE value)
{
	BYTE bytes[2] = { 0x83, 0xEC };
	bp.Write(bytes, 2);
	bp.Write(value);
}
void Assembly::SubEsp(int value)
{
	BYTE bytes[2] = { 0x81, 0xEC };
	bp.Write(bytes, 2);
	bp.WriteT(value);
}
void Assembly::_AddEsp(int value)
{
	if (value > 127 || value < -128)
	{
		AddEsp(value);
	}
	else
	{
		AddEsp((BYTE)value);
	}
}
void Assembly::_Call(void* address)
{
	auto current = (int)this->bp.get_Address() + 5;
	auto addressv = (int)address - current;
	bp.Write(0xE8);
	bp.WriteT(addressv);
}
void Assembly::JMP(BYTE value)
{
	bp.Write(0xEB);
	bp.Write(value);
}
void Assembly::JMP(int value)
{
	bp.Write(0xE9);
	bp.WriteT(value);
}
void Assembly::JGE(BYTE value)
{
	bp.Write(0x7D);
	bp.Write(value);
}
void Assembly::JGE(int value)
{
	BYTE bytes[] = { 0x0F,0x8D };
	bp.Write(bytes,2);
	bp.WriteT(value);
}
void Assembly::JA(BYTE value)
{
	bp.Write(0x77);
	bp.Write(value);
}
void Assembly::JA(int value)
{
	BYTE bytes[] = { 0x0F,0x87 };
	bp.Write(bytes, 2);
	bp.WriteT(value);
}
void Assembly::JB(BYTE value)
{
	bp.Write(0x72);
	bp.Write(value);
}
void Assembly::JAE(BYTE value)
{
	bp.Write(0x73);
	bp.Write(value);
}
void Assembly::JAE(int value)
{
	BYTE bytes[] = { 0x0F,0x83 };
	bp.Write(bytes, 2);
	bp.WriteT(value);
}
void Assembly::JE(BYTE value)
{
	bp.Write(0x74);
	bp.Write(value);
}
void Assembly::JE(int value)
{
	BYTE bytes[] = { 0x0F,0x84 };

	bp.Write(bytes, 2);
	bp.WriteT(value);
}
void Assembly::JNE(BYTE value)
{
	bp.Write(0x75);
	bp.Write(value);
}
void Assembly::JNE(int value)
{
	BYTE bytes[] = { 0x0F,0x85 };

	bp.Write(bytes,2);
	bp.WriteT(value);
}
void Assembly::JBE(BYTE value)
{
	bp.Write(0x76);
	bp.Write(value);
}
void Assembly::Leave()
{
	bp.Write(0xC9);
}
void Assembly::Ret()
{
	bp.Write(0xC3);
}
void Assembly::FLDQWORD(double * address)
{
	BYTE bytes[2] = { 0xDD,0x05};
	
	bp.Write(bytes,2);
	bp.WriteT<double*>(address);

}
void Assembly::fldQWORDEbp(BYTE offset)
{
	BYTE bytes[2] = { 0xDD,0x45 };

	bp.Write(bytes, 2);
	bp.Write(offset);
}
void Assembly::fldQWORDEbp(int offset)
{
	BYTE bytes[2] = { 0xDD,0x85 };

	bp.Write(bytes, 2);
	bp.WriteT(offset);
}
void Assembly::_fldQWORDEbp(int offset)
{
	if (offset > 127 || offset < -128)
	{
		fldQWORDEbp(offset);
	}
	else
	{
		fldQWORDEbp((BYTE)offset);
	}
}
void Assembly::fistpDword_EBP(BYTE offset)
{
	BYTE bytes[2] = { 0xDB,0x5D };

	bp.Write(bytes, 2);
	bp.Write(offset);
}
void Assembly::fstpQWORDEax()
{
	BYTE bytes[2] = { 0xDD,0x18 };

	bp.Write(bytes, 2);
}
void Assembly::fldQWORDEax()
{
	BYTE bytes[2] = { 0xDD,0x00 };

	bp.Write(bytes, 2);
}
void Assembly::fstpQWORDEsp()
{
	BYTE bytes[3] = { 0xDD,0x1C,0x24 };

	bp.Write(bytes, 3);
}
void Assembly::fstpQWORDEbp(BYTE offset)
{
	BYTE bytes[] = { 0xDD,0x5D };

	bp.Write(bytes, 2);
	bp.Write(offset);
}
void Assembly::fstpQWORDEbp(int offset)
{
	BYTE bytes[] = { 0xDD,0x9D };

	bp.Write(bytes, 2);
	bp.WriteT(offset);
}
void Assembly::_fstpQWORDEbp(int offset)
{
	if (offset > 127 || offset < -128)
	{
		fstpQWORDEbp(offset);
	}
	else
	{
		fstpQWORDEbp((BYTE)offset);
	}
}
void Assembly::fstpQWORD(double * address)
{
	BYTE bytes[] = { 0xDD,0x1D };

	bp.Write(bytes, 2);
	bp.WriteT(address);
}
void Assembly::fcomip_st0_st1()
{
	BYTE bytes[] = { 0xDF,0xF1 };

	bp.Write(bytes, 2);
}
void Assembly::Push(BYTE val)
{
	bp.Write(0x6A);
	bp.Write(val);

}
void Assembly::Push(int val)
{
	bp.Write(0x68);
	bp.WriteT(val);
}
void Assembly::_Push(int val)
{
	if (val > 127 || val < -128)
	{
		Push(val);
	}
	else
	{
		Push((BYTE)val);

	}
}
void Assembly::IncEBP(BYTE offset)
{
	BYTE bytes[] = { 0xFF,0x45 };

	bp.Write(bytes, 2);
	bp.Write(offset);
}
DoubleRet Assembly::Compile()
{

	auto const function_ptr = reinterpret_cast<DoubleRet>(bp.begin());
	return function_ptr;
}
void Assembly::PopEbp()
{
	bp.Write(0x5D);
}