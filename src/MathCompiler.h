#pragma once
#include "Light/Arguments.h"
#include "Assembly.h"
#include <unordered_map>
#include <Windows.h>

namespace MathCompiler
{
	class Compiler;
	typedef void(*Callback) (Compiler* compiler,Light::Function& v);
	struct CValue
	{
		enum class Type : BYTE
		{
			None,
			Stack,
			Expression,
			Number,
			Address
		};
		union
		{
			struct
			{
				Light::ResultType Stacktype;
				int stackindex;
			};
			struct
			{
				Light::ResultType Referencetype;
				void* reference;
			};
			double numberval;
			Light::CommandResult comandresult;
		};
		Type cvalueType;
	};
	struct CExpression
	{
		Light::COperator op;
		CValue val1;
		CValue val2;
	};
	struct CVariable
	{
		bool isStack;
		union
		{
			double* v;
			int offset;
		};
	};
	class Compiler
	{
	public:
		Compiler(int sizecode, int sizedata);

		DoubleRet CompileFromInstruction(Light::List& instruction);
		CExpression ToExpression(Light::Array<CValue>& val, size_t current,int& removelen);
		void NumberToFld(CValue& cvalue);
		void ChangeMemoryAccess(DWORD memoryprotection);
		double GetDouble(Light::CommandResult& result);
		Light::string_view* GetString(Light::List& result);

		void Compiler_Constant(Light::string_view& str);
		void Compiler_Function(Light::Function& fn);
		void FromExpression(Light::List& instruction);
		int Macro_NewStackFloat();
		void Macro_PushFloat();
		void SetVariable(Light::string_view& str, CVariable& variablev);
		bool GetVariable(Light::string_view& str, CVariable& variablev);

		void FreeStack(int size);
		int AllocStack(int size);
		void Reset();
		Callback callback;
	private:
		std::unordered_map<Light::string_view, CVariable, Light::StringView_Hash> keys;

		Assembly assembly;
		BytePack data;
		BYTE* start;
		int len;
		int stack;
	};

}