#include "MathCompiler.h"
#include "MathConstantAndVar.h"
#include "MathMethods.h"
BYTE* CreateMemory(int size)
{
	auto const buffer = VirtualAlloc(nullptr, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	return (BYTE*)buffer;
}
MathCompiler::Compiler::Compiler(int sizecode, int sizedata)
{
	auto result = CreateMemory(sizecode + sizecode);
	this->len = sizecode + sizedata;
	this->start = result;
	assembly = Assembly(sizecode,result);
	this->data = BytePack(result+ sizecode, sizedata);
}

DoubleRet MathCompiler::Compiler::CompileFromInstruction(Light::List & instruction)
{
	stack = 0;
	auto current = (DoubleRet)assembly.bp.get_Address();
	assembly.PushEbp();
	assembly.MovEbpEsp();
	auto stacklen = (int*)(assembly.bp.get_Address() + 2);
	assembly.SubEsp(0);
	if (instruction.get_Count() > 0)
	{
		FromExpression(instruction);
	}
	else
	{
		assembly.fldz();
	}
	*stacklen = -stack;
	assembly.Leave();
	assembly.Ret();
	return current;
}

MathCompiler::CExpression MathCompiler::Compiler::ToExpression(Light::Array<CValue>& val, size_t current,int& removelen)
{
	CExpression expression = CExpression();
	auto first = val[current];
	if (first.comandresult.resultType != Light::ResultType::Operador)
	{
		auto second = val[current + 1];

		if (second.comandresult.resultType != Light::ResultType::Operador)
		{
			expression.val1 = first;
			expression.op = Light::COperator::Add;
			expression.val2 = second;
			removelen = 1;

		}
		else
		{
			expression.val1 = first;
			expression.op = *(second.comandresult.operador);
			expression.val2 = val[current + 2];
			removelen = 2;
		}
	}
	else
	{
		expression.val1 = first;
		expression.op = *(first.comandresult.operador);
		expression.val2 = val[current + 1];
		removelen = 1;
	}
	
	return expression;
}
void MathCompiler::Compiler::NumberToFld(CValue& cvalue)
{
	if (cvalue.cvalueType == CValue::Type::Expression)
	{
		auto result = cvalue.comandresult;
		if (result.resultType == Light::ResultType::Number)
		{
			var val = this->GetDouble(cvalue.comandresult);
			if (val == 0.0)
			{
				assembly.fldz();
			}
			else if (val == 1.0)
			{
				assembly.fld1();
			}
			else
			{
				auto current = data.get_AddressT<double>();
				data.WriteT(val);
				assembly.FLDQWORD(current);
			}
		}
		else if (result.resultType == Light::ResultType::Label)
		{
			auto str = result.str;
			if (!str->startWith("$", 0))
			{
				Compiler_Constant(*str);

			}
			else
			{
				auto strm = str->SubString(1);
				CVariable val;
				if (GetVariable(strm,val))
				{
					if (val.isStack)
					{
						assembly._fldQWORDEbp(val.offset);
					}
					else
					{
						assembly.FLDQWORD(val.v);
					}
				}
			}
		}
		else if (result.resultType == Light::ResultType::Function)
		{
			auto fn = result.fn;
			Compiler_Function(*fn);
		}
		else if (result.resultType == Light::ResultType::Expression)
		{
			FromExpression(result.expression->at(0));
		}
	}
	else if (cvalue.cvalueType == CValue::Type::Stack)
	{
		assembly._fldQWORDEbp(cvalue.stackindex);
	}
	else if (cvalue.cvalueType == CValue::Type::Address)
	{
		assembly.FLDQWORD((double*)cvalue.reference);
	}
	else if (cvalue.cvalueType == CValue::Type::Number)
	{
		var val = cvalue.numberval;
		if (val == 0.0)
		{
			assembly.fldz();
		}
		else if (val == 1.0)
		{
			assembly.fld1();
		}
		else
		{
			auto current = data.get_AddressT<double>();
			data.WriteT(val);
			assembly.FLDQWORD(current);
		}
	}
}

void MathCompiler::Compiler::ChangeMemoryAccess(DWORD memoryprotection)
{
	DWORD dummy;
	VirtualProtect(start, len, memoryprotection, &dummy);
}

double MathCompiler::Compiler::GetDouble(Light::CommandResult & result)
{
	if (result.resultType == Light::ResultType::Number)
	{
		return result.ToDouble();
	}
	return 0.0;
}

Light::string_view* MathCompiler::Compiler::GetString(Light::List & result)
{
	return result.at(0).str;
}

void MathCompiler::Compiler::Compiler_Constant(Light::string_view& str)
{
	if (str.Equals("PI"))
	{
		assembly.fldpi();
	}
	else if (str.Equals("LOG2E"))
	{
		assembly.fldl2e();
	}
	else if (str.Equals("LOG2_10"))
	{
		assembly.fldl2t();
	}
	else if (str.Equals("LOG10_2"))
	{
		assembly.fldlg2();
	}
	else if (str.Equals("LN_2"))
	{
		assembly.fldln2();
	}
	else if (str.Equals("TRUE"))
	{
		assembly.fld1();
	}
	else if (str.Equals("FALSE"))
	{
		assembly.fldz();
	}
	else if (str.Equals("NAN"))
	{
		assembly.FLDQWORD(&MathCompiler::ConstantNumbers::NaN);
	}
	else if (str.Equals("INFINITY"))
	{
		assembly.FLDQWORD(&MathCompiler::ConstantNumbers::Infinity);
	}
	else if (str.Equals("E"))
	{
		assembly.FLDQWORD(&MathCompiler::ConstantNumbers::E);
	}
}

void MathCompiler::Compiler::Compiler_Function(Light::Function & fn)
{
	auto fn_name = fn.name.str;
	auto fn_args = fn.args.expression;
	auto fn_argc = fn_args->get_Count();

	if (fn_name->Equals("sqrt"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			assembly.fsqrt();
		}
	}
	else if (fn_name->Equals("abs"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			assembly.fabs();
		}
	}
	else if (fn_name->Equals("round"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			assembly.frndint();
		}
	}
	else if (fn_name->Equals("cos"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			assembly.fcos();
		}
	}
	else if (fn_name->Equals("sin"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			assembly.fsin();
		}
	}
	else if (fn_name->Equals("tan"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			assembly.fptan();
		}
	}
	else if (fn_name->Equals("atan"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			assembly.fpatan();
		}
	}
	else if (fn_name->Equals("mod"))
	{
		if (fn_argc == 2)
		{

			FromExpression(fn_args->at(1));
			Macro_PushFloat();
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::Fmod);
			assembly._AddEsp(16);
		}
	}
	else if (fn_name->Equals("max"))
	{

		if (fn_argc == 1)
		{
			auto arr = fn_args->at(0).at(0).list;
			auto len = arr->get_Count();
			auto current = data.get_AddressT<double>();
			auto temp = current;
			data.set_CurrentIndex(data.get_CurrentIndex() + 8 * len);
			auto enumerator = arr->get_Enumerator();
			while (enumerator.Next())
			{
				CValue v;
				v.cvalueType = CValue::Type::Expression;
				v.comandresult = enumerator.get_Current();
				NumberToFld(v);
				assembly.fstpQWORD(temp++);
			}
			assembly._Push((int)current);
			assembly._Push(len);

			assembly._Call(MathCompiler::Methods::Max);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("min"))
	{
		if (fn_argc == 1)
		{
			auto arr = fn_args->at(0).at(0).list;
			auto len = arr->get_Count();
			auto current = data.get_AddressT<double>();
			auto temp = current;
			data.set_CurrentIndex(data.get_CurrentIndex() + 8 * len);
			auto enumerator = arr->get_Enumerator();
			while (enumerator.Next())
			{
				CValue v;
				v.cvalueType = CValue::Type::Expression;
				v.comandresult = enumerator.get_Current();
				NumberToFld(v);
				assembly.fstpQWORD(temp++);
			}
			assembly._Push((int)current);
			assembly._Push(len);

			assembly._Call(MathCompiler::Methods::Min);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("pow"))
	{
		if (fn_argc == 2)
		{

			FromExpression(fn_args->at(1));
			Macro_PushFloat();
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::Pow);
			assembly._AddEsp(16);
		}
	}
	else if (fn_name->Equals("log"))
	{
		if (fn_argc == 1)
		{

			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::Log);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("acos"))
	{
		if (fn_argc == 1)
		{

			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::Acos);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("asin"))
	{
		if (fn_argc == 1)
		{

			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::Asin);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("cbrt"))
	{
		if (fn_argc == 1)
		{

			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::Cbrt);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("ceil"))
	{
		if (fn_argc == 1)
		{

			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::Ceil);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("cosh"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::Cosh);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("exp"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::Exp);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("expm1"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::Expm1);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("fdim"))
	{
		if (fn_argc == 2)
		{
			FromExpression(fn_args->at(1));
			Macro_PushFloat();
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::Fdim);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("floor"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::Floor);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("sinh"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::Sinh);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("tanh"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::Tanh);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("hypot"))
	{
		if (fn_argc == 2)
		{
			FromExpression(fn_args->at(1));
			Macro_PushFloat();
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::Hypot);
			assembly._AddEsp(16);
		}
	}
	else if (fn_name->Equals("if"))
	{
		if (fn_argc == 3)
		{
			FromExpression(fn_args->at(2));
			Macro_PushFloat();
			FromExpression(fn_args->at(1));
			Macro_PushFloat();
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::If);
			assembly._AddEsp(24);
		}
	}
	else if (fn_name->Equals("summation"))
	{
		if (fn_argc == 4)
		{
			auto variablename = fn_args->at(0).at(0).str->SubString(1);
			assembly.fldz();
			auto somation = Macro_NewStackFloat();
			FromExpression(fn_args->at(1));
			auto i = Macro_NewStackFloat();
			CVariable variable;
			variable.isStack = true;
			variable.offset = i;
			SetVariable(variablename, variable);
			auto comparatev = AllocStack(8);
			auto L1 = assembly.bp.get_Address();

			FromExpression(fn_args->at(2));
			assembly._fldQWORDEbp(i);

			assembly.fcomip_st0_st1();
			
			auto L2 = (int*)(assembly.bp.get_Address() + 2);
			assembly.JAE(0);

			FromExpression(fn_args->at(3));
			assembly._fldQWORDEbp(somation);
			assembly.faddp_st1_st();

			assembly._fstpQWORDEbp(somation);
			// i++
			assembly.fld1();
			assembly._fldQWORDEbp(i);
			assembly.faddp_st1_st();
			assembly._fstpQWORDEbp(i);

			auto L3 = assembly.bp.get_Address() + 5;
			assembly.JMP((int)(L1 - L3));
			*L2 = (int)L3 - ((int)L2 + 4);
			//result
			assembly._fldQWORDEbp(somation);
		}
	}
	else if (fn_name->Equals("bin"))
	{
		if (fn_argc == 1)
		{
			auto str = GetString(fn_args->at(0));
			double v = MathCompiler::Methods::Bin(str->data());
			auto current = data.get_AddressT<double>();
			data.WriteT(v);
			assembly.FLDQWORD(current);
		}
	}
	else if (fn_name->Equals("hex"))
	{
		if (fn_argc == 1)
		{
			auto str = GetString(fn_args->at(0));
			double v = MathCompiler::Methods::Hex(str->data());
			auto current = data.get_AddressT<double>();
			data.WriteT(v);
			assembly.FLDQWORD(current);
		}
	}
	else if (fn_name->Equals("log10"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::Log10);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("log2"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::Log2);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("random"))
	{
		if (fn_argc == 2)
		{
			FromExpression(fn_args->at(1));
			Macro_PushFloat();
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::Random);
			assembly._AddEsp(16);
		}
	}
	else if (fn_name->Equals("sign"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::Sign);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("trunc"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::Trunc);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("isfinite"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::IsFinite);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("isinf"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::IsInf);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("isnan"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::IsNaN);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("isnormal"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::IsNormal);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("intpart"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::IntPart);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("fractpart"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::FractPart);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("clamp"))
	{
		if (fn_argc == 3)
		{
			FromExpression(fn_args->at(2));
			Macro_PushFloat();
			FromExpression(fn_args->at(1));
			Macro_PushFloat();
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::Clamp);
			assembly._AddEsp(24);
		}
	}
	else if (fn_name->Equals("NOT"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::Not);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("degrees"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::Degrees);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("radians"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::Radians);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("fact"))
	{
		if (fn_argc == 1)
		{
			FromExpression(fn_args->at(0));
			Macro_PushFloat();
			assembly._Call(MathCompiler::Methods::Fact);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("average"))
	{
		if (fn_argc == 1)
		{
			auto arr = fn_args->at(0).at(0).list;
			auto len = arr->get_Count();
			auto current = data.get_AddressT<double>();
			auto temp = current;
			data.set_CurrentIndex(data.get_CurrentIndex() + 8 * len);
			auto enumerator = arr->get_Enumerator();
			while (enumerator.Next())
			{
				CValue v;
				v.cvalueType = CValue::Type::Expression;
				v.comandresult = enumerator.get_Current();
				NumberToFld(v);
				assembly.fstpQWORD(temp++);
			}
			assembly._Push((int)current);
			assembly._Push(len);

			assembly._Call(MathCompiler::Methods::Average);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("median"))
	{
		if (fn_argc == 1)
		{
			auto arr = fn_args->at(0).at(0).list;
			auto len = arr->get_Count();
			auto current = data.get_AddressT<double>();
			auto temp = current;
			data.set_CurrentIndex(data.get_CurrentIndex() + 8 * len);
			auto enumerator = arr->get_Enumerator();
			while (enumerator.Next())
			{
				CValue v;
				v.cvalueType = CValue::Type::Expression;
				v.comandresult = enumerator.get_Current();
				NumberToFld(v);
				assembly.fstpQWORD(temp++);
			}
			assembly._Push((int)current);
			assembly._Push(len);

			assembly._Call(MathCompiler::Methods::Median);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("mode"))
	{
		if (fn_argc == 1)
		{
			auto arr = fn_args->at(0).at(0).list;
			auto len = arr->get_Count();
			auto current = data.get_AddressT<double>();
			auto temp = current;
			data.set_CurrentIndex(data.get_CurrentIndex() + 8 * len);
			auto enumerator = arr->get_Enumerator();
			while (enumerator.Next())
			{
				CValue v;
				v.cvalueType = CValue::Type::Expression;
				v.comandresult = enumerator.get_Current();
				NumberToFld(v);
				assembly.fstpQWORD(temp++);
			}
			assembly._Push((int)current);
			assembly._Push(len);

			assembly._Call(MathCompiler::Methods::Mode);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("sum"))
	{
		if (fn_argc == 1)
		{
			auto arr = fn_args->at(0).at(0).list;
			auto len = arr->get_Count();
			auto current = data.get_AddressT<double>();
			auto temp = current;
			data.set_CurrentIndex(data.get_CurrentIndex() + 8 * len);
			auto enumerator = arr->get_Enumerator();
			while (enumerator.Next())
			{
				CValue v;
				v.cvalueType = CValue::Type::Expression;
				v.comandresult = enumerator.get_Current();
				NumberToFld(v);
				assembly.fstpQWORD(temp++);
			}
			assembly._Push((int)current);
			assembly._Push(len);

			assembly._Call(MathCompiler::Methods::Sum);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("range"))
	{
		if (fn_argc == 1)
		{
			auto arr = fn_args->at(0).at(0).list;
			auto len = arr->get_Count();
			auto current = data.get_AddressT<double>();
			auto temp = current;
			data.set_CurrentIndex(data.get_CurrentIndex() + 8 * len);
			auto enumerator = arr->get_Enumerator();
			while (enumerator.Next())
			{
				CValue v;
				v.cvalueType = CValue::Type::Expression;
				v.comandresult = enumerator.get_Current();
				NumberToFld(v);
				assembly.fstpQWORD(temp++);
			}
			assembly._Push((int)current);
			assembly._Push(len);

			assembly._Call(MathCompiler::Methods::Range);
			assembly._AddEsp(8);
		}
	}
	else if (fn_name->Equals("fun"))
	{
		if (fn_argc == 3)
		{
			auto variablename = GetString(fn_args->at(0))->SubString(1);
			FromExpression(fn_args->at(1));
			auto arg0 = Macro_NewStackFloat();
			CVariable variable;
			variable.isStack = true;
			variable.offset = arg0;
			SetVariable(variablename, variable);

			FromExpression(fn_args->at(2));
		}
	}
	else if (fn_name->Equals("switch"))
	{
		auto arrkey = fn_args->at(1).at(0).dictionary;
		auto len = arrkey->get_Count();
		auto current = data.get_AddressT<double>();
		auto temp = current;
		data.set_CurrentIndex(data.get_CurrentIndex() + 16 * len);
		auto enumerator = arrkey->get_Enumerator();
		while (enumerator.Next())
		{
			auto&current = enumerator.get_Current();
			CValue v;
			v.cvalueType = CValue::Type::Expression;
			v.comandresult = current.key;
			NumberToFld(v);
			assembly.fstpQWORD(temp++);
			v.comandresult = current.value;
			NumberToFld(v);
			assembly.fstpQWORD(temp++);
		}
		FromExpression(fn_args->at(2));
		Macro_PushFloat();
		assembly._Push((int)current);
		assembly._Push(len);

		FromExpression(fn_args->at(0));
		Macro_PushFloat();
		assembly._Call(MathCompiler::Methods::Switch);
		assembly._AddEsp(24);

		
	}
	else if (fn_name->Equals("input"))
	{
		if (fn_argc == 0)
		{
			assembly._Call(MathCompiler::Methods::Input);
		}
	}
	else if (callback != NULL)
	{
		callback(this, fn);
	}
}

void MathCompiler::Compiler::FromExpression(Light::List & instruction)
{
	Light::Array<CValue> array_;
	auto len = instruction.get_Count();
	auto enumerator = instruction.get_Enumerator();
	while (enumerator.Next())
	{
		auto current = enumerator.get_Current();
		if (current.resultType == Light::ResultType::Number)
		{
			CValue val;
			val.cvalueType = CValue::Type::Number;
			val.numberval = current.ToDouble();
			array_.Add(val);
		}
		else
		{
			CValue val;
			val.cvalueType = CValue::Type::Expression;
			val.comandresult = current;
			array_.Add(val);
		}
	}
	for (size_t i = 0; i < len - 1; i += 2)
	{
		int oldi = 0;
		auto expression = ToExpression(array_, i, oldi);
		auto op = expression.op;
		auto v1 = expression.val1, v2 = expression.val2;
		if (v1.comandresult.resultType == Light::ResultType::Operador)
		{
			if (op == Light::COperator::Sub)
			{
				if (v2.cvalueType == CValue::Type::Number)
				{
					CValue val;
					val.cvalueType = CValue::Type::Number;
					val.numberval = -v2.numberval;
					array_[i] = val;
				}
				else
				{
					NumberToFld(v2);
					assembly.fchs();
					CValue val;
					val.cvalueType = CValue::Type::Stack;
					val.stackindex = Macro_NewStackFloat();
					val.Stacktype = Light::ResultType::Number;
					array_[i] = val;
				}
				
				array_.RemoveAt(i + 1, oldi);
				len -= oldi;
				i -= 2;
			}
		}
	}
	for (size_t i = 0; i < len - 1; i += 2)
	{
		int oldi = 0;
		auto expression = ToExpression(array_, i, oldi);
		auto op = expression.op;
		auto v1 = expression.val1, v2 = expression.val2;
		if (op == Light::COperator::Mul)
		{
			if (v2.cvalueType == CValue::Type::Number && v1.cvalueType == CValue::Type::Number)
			{
				CValue val;
				val.cvalueType = CValue::Type::Number;
				val.numberval = v1.numberval * v2.numberval;
				array_[i] = val;
			}
			else
			{
				NumberToFld(v1);
				NumberToFld(v2);
				assembly.fmulp_st1_st();
				CValue val;
				val.cvalueType = CValue::Type::Stack;
				val.stackindex = Macro_NewStackFloat();
				val.Stacktype = Light::ResultType::Number;
				array_[i] = val;
			}
			
			array_.RemoveAt(i + 1, oldi);
			len -= oldi;
			i -= 2;
		}
		else if (op == Light::COperator::Div)
		{
			if (v2.cvalueType == CValue::Type::Number && v1.cvalueType == CValue::Type::Number)
			{
				CValue val;
				val.cvalueType = CValue::Type::Number;
				val.numberval = v1.numberval / v2.numberval;
				array_[i] = val;
			}
			else
			{
				NumberToFld(v1);
				NumberToFld(v2);
				assembly.fdivp_st1_st();
				CValue val;
				val.cvalueType = CValue::Type::Stack;
				val.stackindex = Macro_NewStackFloat();
				val.Stacktype = Light::ResultType::Number;
				array_[i] = val;
			}

			array_.RemoveAt(i + 1, oldi);
			i -= 2;
			len -= oldi;
		}
	}
	for (size_t i = 0; i < len - 1; i += 2)
	{
		int oldi = 0;
		auto expression = ToExpression(array_, i, oldi);
		auto op = expression.op;
		auto v1 = expression.val1, v2 = expression.val2;
		if (op == Light::COperator::Add)
		{

			if (v2.cvalueType == CValue::Type::Number && v1.cvalueType == CValue::Type::Number)
			{
				CValue val;
				val.cvalueType = CValue::Type::Number;
				val.numberval = v1.numberval + v2.numberval;
				array_[i] = val;
			}
			else
			{
				NumberToFld(v1);
				NumberToFld(v2);
				assembly.faddp_st1_st();
				CValue val;
				val.cvalueType = CValue::Type::Stack;
				val.stackindex = Macro_NewStackFloat();
				val.Stacktype = Light::ResultType::Number;
				array_[i] = val;
			}

			array_.RemoveAt(i + 1, oldi);
			i -= 2;
			len -= oldi;
		}
		else if (op == Light::COperator::Sub)
		{
			if (v2.cvalueType == CValue::Type::Number && v1.cvalueType == CValue::Type::Number)
			{
				CValue val;
				val.cvalueType = CValue::Type::Number;
				val.numberval = v1.numberval + v2.numberval;
				array_[i] = val;
			}
			else
			{
				NumberToFld(v1);
				NumberToFld(v2);
				assembly.fsubp_st1_st();
				CValue val;
				val.cvalueType = CValue::Type::Stack;
				val.stackindex = Macro_NewStackFloat();
				val.Stacktype = Light::ResultType::Number;
				array_[i] = val;
			}

			array_.RemoveAt(i + 1, oldi);
			i -= 2;
			len -= oldi;
		}
	}
	for (size_t i = 0; i < len - 1; i += 2)
	{
		int oldi = 0;
		auto expression = ToExpression(array_, i, oldi);
		auto op = expression.op;
		auto v1 = expression.val1, v2 = expression.val2;
		if (op == Light::COperator::CmpE)
		{

			if (v2.cvalueType == CValue::Type::Number && v1.cvalueType == CValue::Type::Number)
			{
				CValue val;
				val.cvalueType = CValue::Type::Number;
				val.numberval = v1.numberval == v2.numberval;
				array_[i] = val;
			}
			else
			{
				NumberToFld(v2);
				NumberToFld(v1);
				assembly.fcomip_st0_st1();

				assembly.JNE((BYTE)4);
				assembly.fld1();
				assembly.JMP((BYTE)2);

				assembly.fldz();
				CValue val;
				val.cvalueType = CValue::Type::Stack;
				val.stackindex = Macro_NewStackFloat();
				val.Stacktype = Light::ResultType::Number;
				array_[i] = val;
			}

			array_.RemoveAt(i + 1, oldi);
			i -= 2;
			len -= oldi;
		}
		else if (op == Light::COperator::CmpNE)
		{
			if (v2.cvalueType == CValue::Type::Number && v1.cvalueType == CValue::Type::Number)
			{
				CValue val;
				val.cvalueType = CValue::Type::Number;
				val.numberval = v1.numberval != v2.numberval;
				array_[i] = val;
			}
			else
			{
				NumberToFld(v2);
				NumberToFld(v1);
				assembly.fcomip_st0_st1();

				assembly.JE((BYTE)4);
				assembly.fld1();
				assembly.JMP((BYTE)2);

				assembly.fldz();
				CValue val;
				val.cvalueType = CValue::Type::Stack;
				val.stackindex = Macro_NewStackFloat();
				val.Stacktype = Light::ResultType::Number;
				array_[i] = val;
			}

			array_.RemoveAt(i + 1, oldi);
			i -= 2;
			len -= oldi;
		}
		else if (op == Light::COperator::CmpG)
		{
			if (v2.cvalueType == CValue::Type::Number && v1.cvalueType == CValue::Type::Number)
			{
				CValue val;
				val.cvalueType = CValue::Type::Number;
				val.numberval = v1.numberval > v2.numberval;
				array_[i] = val;
			}
			else
			{
				NumberToFld(v2);
				NumberToFld(v1);
				assembly.fcomip_st0_st1();

				assembly.JBE(4);
				assembly.fld1();
				assembly.JMP((BYTE)2);
				assembly.fldz();
				CValue val;
				val.cvalueType = CValue::Type::Stack;
				val.stackindex = Macro_NewStackFloat();
				val.Stacktype = Light::ResultType::Number;
				array_[i] = val;
			}

			array_.RemoveAt(i + 1, oldi);
			i -= 2;
			len -= oldi;
		}
		else if (op == Light::COperator::CmpL)
		{
			if (v2.cvalueType == CValue::Type::Number && v1.cvalueType == CValue::Type::Number)
			{
				CValue val;
				val.cvalueType = CValue::Type::Number;
				val.numberval = v1.numberval < v2.numberval;
				array_[i] = val;
			}
			else
			{
				NumberToFld(v2);
				NumberToFld(v1);
				assembly.fcomip_st0_st1();

				assembly.JAE((BYTE)4);
				assembly.fld1();
				assembly.JMP((BYTE)2);

				assembly.fldz();
				CValue val;
				val.cvalueType = CValue::Type::Stack;
				val.stackindex = Macro_NewStackFloat();
				val.Stacktype = Light::ResultType::Number;
				array_[i] = val;
			}

			array_.RemoveAt(i + 1, oldi);
			i -= 2;
			len -= oldi;
		}
		else if (op == Light::COperator::CmpLE)
		{
			if (v2.cvalueType == CValue::Type::Number && v1.cvalueType == CValue::Type::Number)
			{
				CValue val;
				val.cvalueType = CValue::Type::Number;
				val.numberval = v1.numberval <= v2.numberval;
				array_[i] = val;
			}
			else
			{
				NumberToFld(v2);
				NumberToFld(v1);
				assembly.fcomip_st0_st1();

				assembly.JA((BYTE)4);
				assembly.fld1();
				assembly.JMP((BYTE)2);
				assembly.fldz();
				CValue val;
				val.cvalueType = CValue::Type::Stack;
				val.stackindex = Macro_NewStackFloat();
				val.Stacktype = Light::ResultType::Number;
				array_[i] = val;
			}

			array_.RemoveAt(i + 1, oldi);
			i -= 2;
			len -= oldi;
		}
		else if (op == Light::COperator::CmpGE)
		{
			if (v2.cvalueType == CValue::Type::Number && v1.cvalueType == CValue::Type::Number)
			{
				CValue val;
				val.cvalueType = CValue::Type::Number;
				val.numberval = v1.numberval >= v2.numberval;
				array_[i] = val;
			}
			else
			{
				NumberToFld(v2);
				NumberToFld(v1);
				assembly.fcomip_st0_st1();

				assembly.JB(4);
				assembly.fld1();
				assembly.JMP((BYTE)2);
				assembly.fldz();
				CValue val;
				val.cvalueType = CValue::Type::Stack;
				val.stackindex = Macro_NewStackFloat();
				val.Stacktype = Light::ResultType::Number;
				array_[i] = val;
			}

			array_.RemoveAt(i + 1, oldi);
			i -= 2;
			len -= oldi;
		}

	}
	for (size_t i = 0; i < len - 1; i += 2)
	{
		int oldi = 0;
		auto expression = ToExpression(array_, i, oldi);
		auto op = expression.op;
		auto v1 = expression.val1, v2 = expression.val2;
		if (op == Light::COperator::And)
		{

			if (v2.cvalueType == CValue::Type::Number && v1.cvalueType == CValue::Type::Number)
			{
				CValue val;
				val.cvalueType = CValue::Type::Number;
				val.numberval = v1.numberval == 1.0 && v2.numberval == 1.0;
				array_[i] = val;
			}
			else
			{
				NumberToFld(v1);
				assembly.fld1();
				assembly.fcomip_st0_st1();
				auto L1 = (int*)(assembly.bp.get_Address()+2);
				assembly.JNE(0);

				NumberToFld(v2);
				assembly.fld1();
				assembly.fcomip_st0_st1();
				assembly.JNE((BYTE)4);
				assembly.fld1();
				assembly.JMP((BYTE)2);
				auto L2 = assembly.bp.get_Address();
				*L1 = (int)L2 - ((int)L1 + 4);
				assembly.fldz();

				CValue val;
				val.cvalueType = CValue::Type::Stack;
				val.stackindex = Macro_NewStackFloat();
				val.Stacktype = Light::ResultType::Number;
				array_[i] = val;
			}

			array_.RemoveAt(i + 1, oldi);
			i -= 2;
			len -= oldi;
		}
		else if (op == Light::COperator::Or)
		{
			if (v2.cvalueType == CValue::Type::Number && v1.cvalueType == CValue::Type::Number)
			{
				CValue val;
				val.cvalueType = CValue::Type::Number;
				val.numberval = v1.numberval == 1.0 || v2.numberval == 1.0;
				array_[i] = val;
			}
			else
			{
				NumberToFld(v1);
				assembly.fld1();
				assembly.fcomip_st0_st1();
				auto L1 = (int*)(assembly.bp.get_Address() + 2);
				assembly.JE(0);

				NumberToFld(v2);
				assembly.fld1();
				assembly.fcomip_st0_st1();
				assembly.JNE((BYTE)4);

				auto L2 = assembly.bp.get_Address();
				*L1 = (int)L2 - ((int)L1 + 4);
				assembly.fld1();
				assembly.JMP((BYTE)2);
				
				assembly.fldz();

				CValue val;
				val.cvalueType = CValue::Type::Stack;
				val.stackindex = Macro_NewStackFloat();
				val.Stacktype = Light::ResultType::Number;
				array_[i] = val;
			}

			array_.RemoveAt(i + 1, oldi);
			i -= 2;
			len -= oldi;
		}
	}
	//  >= jb, <= ja
	if (len == 1)
	{
		NumberToFld(array_[0]);
	}
	array_.Free();
}

int MathCompiler::Compiler::Macro_NewStackFloat()
{
	stack -= 8;
	assembly._fstpQWORDEbp(stack);
	return stack;
}

void MathCompiler::Compiler::Macro_PushFloat()
{
	assembly._SubEsp(8);
	assembly.fstpQWORDEsp();
}

void MathCompiler::Compiler::SetVariable(Light::string_view& str, CVariable& variablev)
{
	auto strv = str.Clone();
	keys[strv] = variablev;

}

bool MathCompiler::Compiler::GetVariable(Light::string_view& str, CVariable& variablev)
{
	auto interator = keys.find(str);
	if (interator != keys.end())
	{
		auto& entry = *interator;
		variablev = entry.second;
		return true;
	}
	return false;
}

void MathCompiler::Compiler::FreeStack(int size)
{
	stack += size;
}

int MathCompiler::Compiler::AllocStack(int size)
{
	stack -= size;
	return stack;
}

void MathCompiler::Compiler::Reset()
{
	for (const auto& v : keys)
	{
		auto c = (Light::string_view)(v.first);
		c.Free();
	}
	keys.clear();
	assembly.bp.set_CurrentIndex(0);
	stack = 0;
	data.set_CurrentIndex(0);
}
