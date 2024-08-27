#pragma once
#include <string>
#include <vector>

// All dumpers require this class
// These classes are important to the DumperLinker
// Classes are required or Linker cannot link to Dumper

#define vec std::vector


class InternalFunctionData
{
public:
	std::string Name;
	uintptr_t FunctionPointer;

	InternalFunctionData() = default;
	InternalFunctionData(const std::string& NameOfFunction, uintptr_t FunctionPtr)
		: Name(NameOfFunction), FunctionPointer(FunctionPtr) {}

};

class InternalPropertyData
{
public:
	std::string Name;
	uintptr_t* ValuePointer;

	InternalPropertyData() = default;
	InternalPropertyData(const std::string& NameOfProperty, uintptr_t* ValuePtr)
		: Name(NameOfProperty), ValuePointer(ValuePtr) {}
};

class ClassLinkingData
{
public:
	std::string Name;
	vec<InternalPropertyData> Properties;
	vec<InternalFunctionData> Functions;

	ClassLinkingData() = default;
	ClassLinkingData(const std::string& NameOfClass)
		: Name(NameOfClass) {}

	void AddProperty(const std::string& NameOfProp, uintptr_t* ValuePointer)
	{
		InternalPropertyData Data = InternalPropertyData(NameOfProp, ValuePointer);
		Properties.emplace_back(Data);
	}
	template<typename FuncType>
	void AddFunction(const std::string& NameOfFunc, FuncType FunctionLambda)
	{
		InternalFunctionData Data = InternalFunctionData(NameOfFunc, FunctionLambda);
		Functions.emplace_back(Data);
	}
};
