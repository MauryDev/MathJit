#pragma once
namespace MathCompiler
{
	namespace Methods
	{
		struct SwitchStruct {
			double key;
			double value;
		};
		double Max(int len, double* buff);
		double Min(int len, double* buff);
		double Pow(double val1, double val2);
		double Fmod(double val1, double val2);
		double Log(double val);
		double Acos(double val);
		double Asin(double val);
		double Cbrt(double val);
		double Ceil(double val);
		double Cosh(double val);
		double Exp(double val);
		double Expm1(double val);
		double Fdim(double val1,double val2);
		double Floor(double val);
		double Hypot(double val1,double val2);
		double Sinh(double val);
		double Tanh(double val);
		double If(double condition,double vt,double vf);
		double Bin(const char* val);
		double Hex(const char* val);
		double Log10(double val);
		double Log2(double val);
		double Random(double min,double max);
		double Sign(double val);
		double IsFinite(double val);
		double Trunc(double val);
		double IsInf(double val);
		double IsNaN(double val);
		double IsNormal(double val);
		double IntPart(double val);
		double FractPart(double val);
		double Clamp(double val, double minval, double maxval);
		double Not(double val);
		double Degrees(double val);
		double Radians(double val);
		double Fact(double val);
		double Average(int len,double* buff);
		double Median(int len, double* buff);
		double Mode(int len, double* buff);
		double Sum(int len, double* buff);
		double Range(int len, double* buff);
		double Switch(double value,int len, SwitchStruct *buff,double defaultvalue);
		double Input();
	}
}