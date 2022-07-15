// BytePackProject.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include "Light/Arguments.h"
#include "Assembly.h"
#include <chrono>
#include <windows.h>
#include "MathCompiler.h"
#include <locale>
class Timer
{
public:
	Timer()
	{
		time = std::chrono::high_resolution_clock::now();
	}
	long long GetIntervalAndReset()
	{
		auto result = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - this->time).count();
		time = std::chrono::high_resolution_clock::now();
		return result;
	}
private:
	std::chrono::time_point<std::chrono::steady_clock> time;
};

MathCompiler::Compiler compile(2000, 2000);
void CompileFromText(std::string& str)
{

	long long compileT = 0, tokenizT = 0, executionT = 0;
	Timer timer;
	Light::string_view view(str.data());
	var arguments = Light::GetInstructions(view);
	tokenizT = timer.GetIntervalAndReset();
	if (arguments != NULL && arguments->get_Count() > 0)
	{
		auto result = compile.CompileFromInstruction(*arguments->begin());
		compileT = timer.GetIntervalAndReset();
		std::cout << "Address " << result << std::endl;
		timer.GetIntervalAndReset();
		auto v = result();
		executionT = timer.GetIntervalAndReset();
		std::cout << str << " = " << v << std::endl;
	}
	std::cout << "Token Time: " << tokenizT << " ns" << std::endl;
	std::cout << "Compiler Time: " << compileT << " ns" << std::endl;

	std::cout << "Execution Time: " << executionT << " ns" << std::endl;
	if (arguments != NULL)
	{
		Light::Free(*arguments);
		delete arguments;
	}

	compile.Reset();
	
}
int main(int argc,char** args)
{
	while (true)
	{
		std::string txt;
		std::cout << "Digite uma expressao numerica" << std::endl;
		std::getline(std::cin, txt);
		CompileFromText(txt);

	}
}