#include <string.h>
#include <sstream> 
#include "Arguments.h"
namespace Light
{

	void FreeFromType(Light::CommandResult &v)
	{
	
		auto type = v.resultType;
		switch (type)
		{
		case Light::ResultType::Label:
		case Light::ResultType::String:
		case Light::ResultType::Number:
		{
			v.str->Free();
			delete v.str;
		}
		break;
		case Light::ResultType::Operador:
		{
			delete v.operador;
		}
		break;
		case Light::ResultType::Expression:
		{
			auto exp = v.expression;
			Free(*exp);

			delete exp;
		}
		break;
		case Light::ResultType::List:
		{
			auto list = v.list;
			auto enumerator = list->get_Enumerator();
			while (enumerator.Next())
			{
				FreeFromType(enumerator.get_Current());
			}
			list->Free();
			delete list;
		}
		break;
		case Light::ResultType::Dictionary:
		{
			auto dc = v.dictionary;
			for (auto current : *dc)
			{
				FreeFromType(current.key);
				FreeFromType(current.value);

			}
			dc->Free();
			delete dc;
		}
		break;
		case Light::ResultType::Function:
		{
			auto fn = v.fn;

			FreeFromType(fn->name);
			FreeFromType(fn->args);
			delete fn;
		}
		break;
		}
	}
	void Free(Light::Expression &v)
	{
		auto enumeratorexp = v.get_Enumerator();
		while (enumeratorexp.Next())
		{
			var& current = enumeratorexp.get_Current();
			auto enumerator = current.get_Enumerator();
			while (enumerator.Next())
			{
				FreeFromType(enumerator.get_Current());

			}
			current.Free();
		}
		v.Free();
	}
	ResultNext TryNext(Light::string_view& str, int index, CommandResult* result)
	{
		ResultNext ret;
		if (index == -1)
		{
			ret.index = -1;
			ret.resultType = ResultType::Undefined;
			return ret;
		}
		auto len = str.size();
		auto data = str.data();
		void* ptr = NULL;
		for (int i = index; i < len; i++)
		{
			auto currentchar = str[i];
			var freestr = len - i;
			
			if (isspace(currentchar))
			{
				index++;
				continue;
			}
			if (memcpy_custom(&data[i], "//", freestr, 2))
			{
				index = str.find('\n', i);
				if (index == -1)
				{
					break;
				}
				else
				{
					continue;
				}
			}
			if (currentchar == '\'')
			{
				index++;
				auto resultlabel = GetLabel(str, index);

				CommandResult val;
				val.resultType = ResultType::Label;
				val.value = resultlabel;
				*result = val;

				ret.index = index;
				ret.resultType = ResultType::Label;
				return ret;

			}
			else if (IsSymbolStart(currentchar))
			{
				auto resultlabel = GetLabel2(str, index);

				CommandResult val;
				val.resultType = ResultType::Label;
				val.value = resultlabel;
				*result = val;

				ret.index = index;
				ret.resultType = ResultType::Label;
				return ret;
			}
			else if (isOperator(str, index, &ptr))
			{
				CommandResult val;
				val.resultType = ResultType::Operador;
				val.value = ptr;
				*result = val;

				ret.index = index;

				ret.resultType = ResultType::Operador;
				return ret;
			}
			else if (currentchar == ';')
			{
				ret.index = i;
				ret.resultType = ResultType::EndInstruction;
				return ret;
			}
			else if (currentchar == '"')
			{
				index++;
				auto resultlabel = GetString(str, index);
				CommandResult val;
				val.resultType = ResultType::String;
				val.value = resultlabel;
				*result = val;
				ret.index = index;
				ret.resultType = ResultType::String;
				return ret;
			}
			else if (isdigit(currentchar) || currentchar == '-' || currentchar == '+')
			{
				auto val = GetNumber(str, index);
				CommandResult val3;
				val3.resultType = ResultType::Number;
				val3.value = val;
				*result = val3;

				ret.index = index;
				ret.resultType = ResultType::Number;
				return ret;
			}
			else if (currentchar == '(')
			{
				index++;
				auto resultlabel = GetExpression2(str, index);
				CommandResult val3;
				val3.resultType = ResultType::Expression;
				val3.value = resultlabel;
				*result = val3;
				ret.index = index;
				ret.resultType = ResultType::Expression;
				return ret;
			}
			else if (currentchar == '[')
			{
				index++;
				auto resultlabel = GetList(str, index);
				CommandResult val3;
				val3.resultType = ResultType::List;
				val3.value = resultlabel;
				*result = val3;
				ret.index = index;
				ret.resultType = ResultType::List;
				return ret;
			}
			else if (currentchar == '{')
			{
				index++;
				auto resultlabel = GetDictionary(str, index);
				CommandResult val3;
				val3.resultType = ResultType::Dictionary;
				val3.value = resultlabel;
				*result = val3;
				ret.index = index;
				ret.resultType = ResultType::Dictionary;
				return ret;

			}
			else if (currentchar == '#')
			{
				index++;
				auto resultlabel = GetFunction(str, index);
				CommandResult val3;
				val3.resultType = ResultType::Function;
				val3.value = resultlabel;
				*result = val3;
				ret.index = index;
				ret.resultType = ResultType::Function;
				return ret;
			}
			else if (!isspace(currentchar))
			{
				throw "Invalid char " + currentchar;
			}
		}

		ret.index = -1;
		ret.resultType = ResultType::Undefined;
		return ret;
	}

	ResultNext TryNext(Light::string_view& str, int index, Light::List* table)
	{
		CommandResult result;
		result.resultType = ResultType::Undefined;


		auto ret = TryNext(str, index, &result);
		if (result.resultType != ResultType::Undefined && result.resultType != ResultType::EndInstruction)
		{
			table->Add(result);
		}
		return ret;
	}
	bool IgnoredChar(char val)
	{
		return val == ')' || val == ';' || val == ']' || isspace(val);
	}
	bool memcpy_custom(const char * ptr, const char * ptr2, int size, int size2)
	{
		return (size2 <= size ? memcmp(ptr, ptr2, size2) == 0 : false);
	}
	bool isOperator(Light::string_view& val, int& index, void** result)
	{

		if (index == -1 || index >= val.size())
		{
			return false;
		}
		auto freev = val.size() - index;

		COperator op = COperator::None;
		if (freev >= 2)
		{
			if (val.startWith("&&",index))
			{
				op = COperator::And;
			}
			else if (val.startWith("||", index))
			{
				op = COperator::Or;
			}
			else if (val.startWith("!=", index))
			{
				op = COperator::CmpNE;
			}
			else if (val.startWith("==", index))
			{
				op = COperator::CmpE;
			}
			else if (val.startWith(">=", index))
			{
				op = COperator::CmpGE;
			}
			else if (val.startWith("<=", index))
			{
				op = COperator::CmpLE;
			}
			else if (val.startWith("++", index))
			{
				op = COperator::Inc;
			}
			else if (val.startWith("--", index))
			{
				op = COperator::Dec;
			}

			if (op != COperator::None)
			{
				index += 2;
				*result = new COperator(op);
				return true;
			}
		}
		var data = &val[index];
		auto current = *data;
		if (current == '+' || current == '-')
		{
			if (freev == 1 || (freev > 1 && !isdigit(val[index + 1])))
			{
				++index;
				*result = new COperator(current == '+' ? COperator::Add : COperator::Sub);
				return true;
			}
			return false;
		}
		if (current == '%')
		{
			op = COperator::Mod;
		}
		else if (current == '=')
		{
			op = COperator::Set;
		}
		else if (current == '*')
		{
			op = COperator::Mul;
		}
		else if (current == '/')
		{
			op = COperator::Div;
		}
		else if (current == '>')
		{
			op = COperator::CmpG;
		}
		else if (current == '<')
		{
			op = COperator::CmpL;
		}

		if (op != COperator::None)
		{
			index++;
			*result = new COperator(op);
			return true;
		}
		else
		{
			return false;
		}
	}
	bool IsSymbolStart(char val)
	{
		return isalpha(val) || val == '_' || val == '$';
	}
	bool IsSymbol(char val)
	{
		return isalnum(val) || val == '_' || val == '$' || val == '.';
	}

	bool FindCaracterNoSpace(Light::string_view& str, int& index, char caracter)
	{
		if (index == -1)
		{
			return false;
		}
		auto len = str.size();
		for (int i = index; i < len; i++)
		{
			auto current = str[i];
			if (isspace(current))
			{
				continue;
			}
			else if (current == caracter)
			{
				index = i;
				return true;
			}
			else
			{
				index = i;
				return false;
			}
		}
		index = -1;
		return false;
	}
	bool FindSeparatorCollection(Light::string_view& str, int& index)
	{
		return FindCaracterNoSpace(str, index, ',');
	}

	bool FindEndDictionary(Light::string_view& str, int& index)
	{
		return FindCaracterNoSpace(str, index, '}');

	}
	bool FindEndList(Light::string_view& str, int& index)
	{
		return FindCaracterNoSpace(str, index, ']');

	}

	bool FindEndExpression(Light::string_view& str, int& index)
	{
		return FindCaracterNoSpace(str, index, ')');

	}
	Light::string_view* GetString(Light::string_view& str, int& index)
	{
		bool on = true;
		var result = Light::StringBuffer();
		while (on)
		{
			if (index >= str.size())
			{
				throw "Invalid String";
			}
			else
			{
				var current = str[index];

				if (current == '\\')
				{
					if (index + 1 >= str.size())
					{
						throw "Invalid String";
					}
					auto val = str[index + 1];
					if (val == 'n')
					{
						result.Add('\n');
					}
					else if (val == 't')
					{
						result.Add('\t');
					}
					else if (val == 'r')
					{
						result.Add('\r');
					}
					else if (val == 'v')
					{
						result.Add('\v');
					}
					else if (val == 'f')
					{
						result.Add('\f');
					}
					else if (val == 'a')
					{
						result.Add('\a');
					}
					else if (val == 'b')
					{
						result.Add('\b');
					}
					else
					{
						result.Add(val);
					}
					index += 2;
				}
				else if (current == '"')
				{
					index++;
					on = false;
				}
				else
				{
					result.Add(current);
					index++;
				}
			}
		}
		result.Add(0);
		return new Light::string_view(result.ToString());
	}
	Light::string_view* GetLabel(Light::string_view& str, int& index)
	{
		bool on = true;
		var result = Light::StringBuffer();
		while (on)
		{
			if (index >= str.size())
			{
				throw "Invalid String";
			}
			else
			{
				var current = str[index];
				if (current == '\\')
				{
					if (index + 1 >= str.size())
					{
						throw "Invalid String";
					}
					auto val = str[index + 1];
					if (val == 'n')
					{
						result.Add('\n');
					}
					else if (val == 't')
					{
						result.Add('\t');
					}
					else if (val == 'r')
					{
						result.Add('\r');
					}
					else if (val == 'v')
					{
						result.Add('\v');
					}
					else if (val == 'f')
					{
						result.Add('\f');
					}
					else if (val == 'a')
					{
						result.Add('\a');
					}
					else if (val == 'b')
					{
						result.Add('\b');
					}
					else
					{
						result.Add(val);
					}
					index += 2;
				}
				else if (current == '\'')
				{
					index++;
					on = false;
				}
				else
				{
					result.Add(current);
					index++;
				}
			}
		}
		result.Add(0);
		return new Light::string_view(result.ToString());
	}
	Light::string_view* GetLabel2(Light::string_view& str, int& index)
	{
		bool on = true;
		var result = Light::StringBuffer();
		bool write = false;
		while (on)
		{
			if (index >= str.size())
			{

				if (write)
				{
					on = false;
					break;
				}
				else
				{
					throw "Invalid String";

				}
			}
			else
			{
				auto current = str[index];
				if (IsSymbol(current))
				{
					result.Add(current);
					write = true;
				}
				else if (write)
				{
					on = false;
					break;
				}
				else
				{
					break;
				}
				index++;
			}
		}
		result.Add(0);
		return new Light::string_view(result.ToString());
	}
	Light::string_view* GetNumber(Light::string_view& str, int& index)
	{
		bool on = true;
		bool numberadd = false;
		bool pontoadd = false;
		bool numberaddpoint = false;
		
		var result = Light::StringBuffer();

		while (on)
		{
			if (index >= str.size())
			{
				on = false;
				if (numberadd && !(pontoadd && !numberaddpoint))
				{
					result.Add(0);
					return new Light::string_view(result.ToString());
				}
				else
				{
					throw "Invalid Number";
				}

			}
			else
			{
				auto current = str[index];
				if (isdigit(current))
				{
					result.Add(current);
					numberadd = true;
					if (pontoadd)
					{
						numberaddpoint = true;
					}
				}
				else if (current == '-' || current == '+' && !numberadd)
				{
					result.Add(current);
				}
				else if (current == '.' && numberadd && !pontoadd)
				{
					result.Add(current);
				}
				else if (numberadd && !(pontoadd && !numberaddpoint))
				{
					on = false;
					break;
				}
				else
				{
					throw "Invalid Number";
				}
				index++;
			}
		}
		result.Add(0);
		return new Light::string_view(result.ToString());
	}
	Expression* GetExpression2(Light::string_view& str, int& index)
	{
		bool sucess = false;
		bool first = true;
		Expression* ret = new Expression;

		while (true)
		{
			ResultNext result = ResultNext();
			if (index != -1 && index < str.size() && FindEndExpression(str, index))
			{
				index = index + 1;
				sucess = true;
				break;
			}
			if (!first)
			{
				if (FindSeparatorCollection(str, index))
				{
					index++;

					ret->Add(Light::List());
					Light::List& last = ret->at(ret->get_Count() - 1);
					result = TryNext(str, index, &last);

					index = result.index;
					first = false;
				}
				else
				{
					Light::List& last = ret->last();
					result = TryNext(str, index, &last);

					index = result.index;
					first = false;
				}
			}
			else if (first)
			{
				ret->Add(Light::List());

				Light::List& last = ret->at(ret->get_Count() - 1);
				result = TryNext(str, index, &last);

				index = result.index;
				first = false;
			}
			else
			{
				break;
			}


			if (index == -1 || result.resultType == ResultType::Undefined || result.resultType == ResultType::EndInstruction)
			{
				throw "Bad Type";

			}
		}
		if (!sucess)
		{
			throw "No sucess List";
		}

		return ret;
	}
	List* GetList(Light::string_view& str, int& index)
	{
		bool sucess = false;
		bool first = true;
		List* ret = new List;
		while (true)
		{
			ResultNext result = ResultNext();
			if (index != -1 && index < str.size() && FindEndList(str, index))
			{
				index = index + 1;
				sucess = true;
				break;
			}
			if (!first && FindSeparatorCollection(str, index))
			{
				index++;
				result = TryNext(str, index, ret);
				index = result.index;
				first = false;
			}
			else if (first)
			{
				result = TryNext(str, index, ret);
				index = result.index;
				first = false;
			}
			else
			{
				break;
			}


			if (index == -1 || result.resultType == ResultType::Undefined || result.resultType == ResultType::EndInstruction)
			{
				throw "Bad Type";

			}
		}
		if (!sucess)
		{
			throw "No sucess List";
		}

		return ret;
	}
	Expression* GetInstructions(Light::string_view& str)
	{
		int index = 0;
		Expression* ret = new Expression();
		auto size = str.size();
		CommandResult cresult = CommandResult();
		bool first = true;
		List* last = NULL;
		while (true)
		{
			ResultNext result = TryNext(str, index, &cresult);
			index = result.index;
			if (index == -1 && index >= size)
			{
				break;
			}
			if (first)
			{
				first = false;
				ret->Add(Light::List());
				last = &ret->last();

			}
			if (result.resultType == ResultType::EndInstruction)
			{
				ret->Add(Light::List());
				last = &ret->last();
				index++;
			}
			else
			{
				//Light::List& last = ret->last();
				last->Add(cresult);
			}
		}

		return ret;
	}
	double CommandResult::ToDouble()
	{
		char* end = NULL;
		return (this->resultType == ResultType::Number ? strtod(str->data(), &end) : 0.0);
	}
	int CommandResult::ToInt()
	{
		char* end = NULL;
		return (this->resultType == ResultType::Number ? strtol(str->data(),&end,10) : 0);
	}

	KeyValuePair GetKeyValue(Light::string_view& str, int& index)
	{
		KeyValuePair ret;

		auto a1 = TryNext(str, index, &ret.key);
		if (a1.index == -1 || a1.resultType == ResultType::Undefined || a1.resultType == ResultType::EndInstruction)
		{
			throw "Bad Type";
		}
		index = a1.index;

		auto istwopoint = FindCaracterNoSpace(str, index, ':');
		if (!istwopoint)
		{
			throw "Bad Type";
		}
		index++;
		auto a2 = TryNext(str, index, &ret.value);
		if (a2.index == -1 || a2.resultType == ResultType::Undefined || a2.resultType == ResultType::EndInstruction)
		{
			throw "Bad Type";
		}
		index = a2.index;
		return ret;
	}
	Dictionary* GetDictionary(Light::string_view& str, int& index)
	{
		bool sucess = false;
		bool first = true;
		Dictionary* ret = new Dictionary();
		while (true)
		{
			if (index != -1 && index < str.size())
			{
				if (FindEndDictionary(str, index))
				{
					index = index + 1;
					sucess = true;
					break;
				}
				if (!first && FindSeparatorCollection(str, index))
				{
					index++;
					auto val = GetKeyValue(str, index);
					ret->Add(val);
					first = false;
				}
				else if (first)
				{
					auto val = GetKeyValue(str, index);
					ret->Add(val);
					first = false;
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}

		}
		if (!sucess)
		{
			throw "No sucess List";
		}

		return ret;
	}
	Function * GetFunction(Light::string_view& str, int & index)
	{
		Function* fn = new Function();
		Light::CommandResult result;
		var resultnext = TryNext(str, index, &result);

		if (resultnext.index != -1 && result.resultType != ResultType::EndInstruction)
		{
			index = resultnext.index;
			fn->name = result;
		}
		else
		{
			throw "Function Invalid";
		}
		resultnext = TryNext(str, index, &result);
		if (resultnext.index != -1 && resultnext.resultType == ResultType::Expression)
		{
			index = resultnext.index;
			fn->args = result;
		}
		else
		{
			throw "Function Invalid";
		}
		return fn;
	}
}
