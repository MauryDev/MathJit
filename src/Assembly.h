#pragma once
#include "BytePack.h"
typedef double(*DoubleRet)();
class Assembly
{
public:
	Assembly();
	Assembly(int size,BYTE* buff);
	void MovPTRESP(int val);
	void Nop();
	void PushEbp();
	void PopEbp();
	void MovEbpEsp();
	void fmulp_st1_st();
	void faddp_st1_st();
	void fsubp_st1_st();
	void fdivp_st1_st();
	void fldz();
	void fld1();
	void fldpi();
	void fldl2e();
	void fldl2t();
	void fldlg2();
	void fldln2();
	void fsqrt();
	void fabs();
	void frndint();
	void fcos();
	void fsin();
	void fsincos();
	void fptan();
	void fpatan();
	void fchs();
	void movEspEbp();
	void movEaxEBP(BYTE offset);
	void CmpEbpEax(BYTE offset);
	void _SubEsp(int value);
	void SubEsp(BYTE value);
	void SubEsp(int value);

	void _AddEsp(int value);
	void AddEsp(BYTE value);
	void AddEsp(int value);

	void LeaEaxEbp(BYTE value);
	void MovEBP(BYTE offset, int value);
	void _Call(void* address);
	void JMP(BYTE value);
	void JMP(int value);
	void JGE(BYTE value);
	void JGE(int value);
	void JA(BYTE value);
	void JA(int value);
	void JB(BYTE value);
	void JAE(BYTE value);
	void JAE(int value);
	void JE(BYTE value);
	void JE(int value);
	void JNE(BYTE value);
	void JNE(int value);

	void JBE(BYTE value);

	void Leave();
	void Ret();
	void FLDQWORD(double* address);

	void fldQWORDEbp(BYTE offset);
	void fldQWORDEbp(int offset);
	void _fldQWORDEbp(int offset);

	void fistpDword_EBP(BYTE offset);

	void fstpQWORDEax();
	void fldQWORDEax();
	void fstpQWORDEsp();
	void fstpQWORDEbp(BYTE offset);
	void fstpQWORDEbp(int offset);
	void _fstpQWORDEbp(int offset);

	void fstpQWORD(double* address);

	void fcomip_st0_st1();

	void Push(BYTE val);
	void Push(int val);
	void _Push(int val);

	void IncEBP(BYTE offset);
	DoubleRet Compile();
	BytePack bp;
};

