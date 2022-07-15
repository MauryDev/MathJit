#pragma once
#define var auto
#include "Literal.h"
namespace Light
{
	
	enum class COperator :int
	{
		None,
		Add, // +
		Sub, // -
		Div, // /
		Mul, // *
		Mod, // %
		And, // &&
		Or, // ||
		CmpE, // ==
		CmpNE, // !=
		CmpG, // >
		CmpL, // <
		CmpGE, // >=
		CmpLE, // <=
		Inc, // ++
		Dec, // --
		Set // =
	};

	enum class ResultType : int
	{
		Undefined,
		Label,
		String,
		Number,
		EndInstruction,
		Expression,
		Operador,
		List,
		Dictionary,
		Function
	};
	struct CommandResult;
	struct KeyValuePair;
	struct Function;
	typedef Light::Array<CommandResult> List;
	typedef Light::Array<List> Expression;
	typedef Light::Array<KeyValuePair> Dictionary;

	struct CommandResult
	{
		ResultType resultType;
		union
		{
			void* value;
			Light::string_view* str;
			COperator* operador;
			Expression* expression;
			List* list;
			Function* fn;
			Dictionary* dictionary;
		};
		double ToDouble();
		int ToInt();
	};
	struct Function
	{
		Light::CommandResult name;
		Light::CommandResult args;
	};
	struct KeyValuePair
	{
		Light::CommandResult key;
		Light::CommandResult value;
	};
	struct ResultNext
	{
		int index;
		ResultType resultType;
	};
	bool memcpy_custom(const char* ptr, const char* ptr2, int size, int size2);
	bool isOperator(Light::string_view& val, int& index, void** result);
	bool IsSymbolStart(char val);
	Expression* GetInstructions(Light::string_view& str);
	void Free(Expression &v);
	Light::string_view* GetLabel(Light::string_view& str, int& index);
	Light::string_view* GetString(Light::string_view& str, int& index);
	Light::string_view* GetLabel2(Light::string_view& str, int& index);
	Light::string_view* GetNumber(Light::string_view& str, int& index);
	Expression* GetExpression2(Light::string_view& str, int& index);
	List* GetList(Light::string_view& str, int& index);
	Dictionary* GetDictionary(Light::string_view& str, int& index);
	Function* GetFunction(Light::string_view& str, int& index);

}