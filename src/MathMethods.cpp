#include "MathMethods.h"
#include "Light/Literal.h"
#include <cmath>
#include <random>
#include <iostream>
namespace MathCompiler
{
	namespace Methods
	{
		std::random_device rd;
		std::mt19937 mt(rd());
	}
}


double MathCompiler::Methods::Max(int len, double * buff)
{
	auto interator = Light::Array_Enumerator<double>(buff, buff + len);
	if (interator.Next())
	{
		double maxv = interator.get_Current();
		while (interator.Next())
		{
			auto v = interator.get_Current();
			if (v > maxv)
			{
				maxv = v;
			}
		}
		return maxv;
	}
	else
	{
		return 0.0;
	}
}

double MathCompiler::Methods::Min(int len, double * buff)
{
	auto interator = Light::Array_Enumerator<double>(buff, buff + len);
	if (interator.Next())
	{
		double minv = interator.get_Current();
		while (interator.Next())
		{
			auto v = interator.get_Current();
			if (v < minv)
			{
				minv = v;
			}
		}
		return minv;
	}
	else
	{
		return 0.0;
	}
}

double MathCompiler::Methods::Pow(double val1, double val2)
{
	return std::pow(val1,val2);
}

double MathCompiler::Methods::Fmod(double val1, double val2)
{
	return std::fmod(val1,val2);
}

double MathCompiler::Methods::Log(double val)
{
	return std::log(val);
}

double MathCompiler::Methods::Acos(double val)
{
	return std::acos(val);
}

double MathCompiler::Methods::Asin(double val)
{
	return std::asin(val);
}

double MathCompiler::Methods::Cbrt(double val)
{
	return std::cbrt(val);
}

double MathCompiler::Methods::Ceil(double val)
{
	return std::ceil(val);
}

double MathCompiler::Methods::Cosh(double val)
{
	return std::cosh(val);
}

double MathCompiler::Methods::Exp(double val)
{
	return std::exp(val);
}

double MathCompiler::Methods::Expm1(double val)
{
	return std::expm1(val);
}

double MathCompiler::Methods::Fdim(double val1, double val2)
{
	return std::fdim(val1,val2);
}

double MathCompiler::Methods::Floor(double val)
{
	return std::floor(val);
}

double MathCompiler::Methods::Hypot(double val1,double val2)
{
	return std::hypot(val1,val2);
}

double MathCompiler::Methods::Sinh(double val)
{
	return std::sinh(val);
}

double MathCompiler::Methods::Tanh(double val)
{
	return std::tanh(val);
}

double MathCompiler::Methods::If(double condition, double vt, double vf)
{
	return condition != 0.0 ? vt : vf;
}

double MathCompiler::Methods::Bin(const char * val)
{
	return (double)strtol(val,nullptr,2);
}
double MathCompiler::Methods::Hex(const char * val)
{
	return (double)strtol(val, nullptr, 16);
}

double MathCompiler::Methods::Log10(double val)
{
	return log10(val);
}

double MathCompiler::Methods::Log2(double val)
{
	return log2(val);
}

double MathCompiler::Methods::Random(double min, double max)
{
	std::uniform_real_distribution<double> dist(min, max);

	return dist(mt);
}

double MathCompiler::Methods::Sign(double val)
{
	return (val > 0.0) ? 1.0 : ((val < 0.0) ? -1.0 : 0.0);

}

double MathCompiler::Methods::IsFinite(double val)
{
	return isfinite(val) ? 1.0 : 0.0;
}

double MathCompiler::Methods::Trunc(double val)
{
	return std::trunc(val);
}

double MathCompiler::Methods::IsInf(double val)
{
	return isinf(val) ? 1.0 : 0.0;
}

double MathCompiler::Methods::IsNaN(double val)
{
	return isnan(val) ? 1.0 : 0.0;
}

double MathCompiler::Methods::IsNormal(double val)
{
	return isnormal(val) ? 1.0 : 0.0;
}

double MathCompiler::Methods::IntPart(double val)
{
	double vali;
	modf(val, &vali);
	return vali;
}

double MathCompiler::Methods::FractPart(double val)
{
	double vali;
	return modf(val, &vali);
}

double MathCompiler::Methods::Clamp(double val, double minval, double maxval)
{
	if (val >= maxval)
	{
		return maxval;
	}
	else if (val <= minval)
	{
		return minval;
	}
	else
	{
		return val;
	}
}

double MathCompiler::Methods::Not(double val)
{
	return val != 0.0 ? 0.0 : 1.0;
}

double MathCompiler::Methods::Degrees(double val)
{
	return val * (180.0 / 3.14159265358979323846);
}

double MathCompiler::Methods::Radians(double val)
{
	return val * (3.14159265358979323846 / 180.0);
}

double MathCompiler::Methods::Fact(double val)
{
	auto num2 = trunc(val);

	if (num2 == -1.0)
	{
		return val;
	}
	if (num2 == 1.0 || num2 == 0.0)
	{
		return 1.0;
	}
	auto abs_value = abs(num2);
	auto ret = 1.0;
	for (size_t i = 1; i <= abs_value; i++)
	{
		ret *= i;
	}
	return (val < 0.0 ? -ret : ret);
}

double MathCompiler::Methods::Average(int len, double * buff)
{
	
	if (len > 0)
	{
		auto ret = *buff;
		for (size_t i = 1; i < len; i++)
		{
			ret += buff[i];
		}
		return ret / len;

	}
	else
	{
		return 0.0;
	}
}

double MathCompiler::Methods::Median(int len, double * buff)
{
	size_t tamanho = len;

	if (tamanho > 0)
	{
		auto tamanhodiv = tamanho / 2;
		std::sort(buff, buff + len);
		return (tamanho % 2 == 0 ?
			(buff[tamanhodiv - 1] + buff[tamanhodiv]) / 2.0 :
			buff[tamanhodiv]
			);
	}
	return 0.0;
}

double MathCompiler::Methods::Mode(int len, double * buff)
{
	if (len > 0)
	{
		std::sort(buff, buff + len);
		auto interator = Light::Array_Enumerator<double>(buff, buff + len);

		double m0 = 0.0, val = 0.0;
		unsigned m0count = 0, count = 0;
		while (interator.Next())
		{
			auto valor_atual = interator.get_Current();
			if (valor_atual != val)
			{
				val = valor_atual;
				count = 1;
			}
			else
			{
				count++;
				if (count > m0count)
				{
					m0count = count;
					m0 = val;
				}
			}
		}
		return m0;
	}
	else
	{
		return 0.0;
	}
}

double MathCompiler::Methods::Sum(int len, double * buff)
{
	auto interator = Light::Array_Enumerator<double>(buff, buff + len);
	auto result = 0.0;
	while (interator.Next())
	{
		result += interator.get_Current();
	}
	return result;
}

double MathCompiler::Methods::Range(int len, double * buff)
{
	if (len == 1)
	{
		return *buff;
	}
	auto interator = Light::Array_Enumerator<double>(buff, buff + len);
	if (interator.Next())
	{
		double maxv = interator.get_Current();
		double minv = maxv;
		while (interator.Next())
		{
			auto v = interator.get_Current();
			if (v > maxv)
			{
				maxv = v;
			}
			if (v < minv)
			{
				minv = v;
			}
		}
		return maxv - minv;
	}
	return 0.0;
}

double MathCompiler::Methods::Switch(double value, int len, SwitchStruct * buff, double defaultvalue)
{
	if (len <= 0)
	{
		return 0.0;
	}
	else
	{
		for (size_t i = 0; i < len; i++)
		{
			auto switchcurrent = buff[i];
			if (switchcurrent.key == value)
			{
				return switchcurrent.value;
			}
		}
		return defaultvalue;
	}
}

double MathCompiler::Methods::Input()
{
	double val = 0.0;
	std::cout << "> ";
	std::cin >> val;
	return val;
}

