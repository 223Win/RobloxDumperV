#pragma once
#include <functional>
#include <string>
#include <vector>

// All dumpers require this class
// These classes are important to the DumperLinker
// Classes are required or Linker cannot link to Dumper

#define vec std::vector
#define str5 std::string

struct InternalFunctionData
{
	std::string Name;
	uintptr_t FunctionPointer;
};

struct InternalPropertyData
{
	std::string Name;
	uintptr_t ValuePointer;
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

	template<typename PropType>
	void AddProperty(const std::string& NameOfProp, PropType Value)
	{
		InternalPropertyData Data{ NameOfProp, reinterpret_cast<uintptr_t>(new PropType(Value)) };
		Properties.emplace_back(Data);
	}

	template<typename FuncType>
	void AddFunction(const std::string& NameOfFunc, std::function<FuncType> FunctionLambda)
	{
		auto FuncPtr = new std::function<FuncType>(FunctionLambda);
		InternalFunctionData Data{ NameOfFunc, reinterpret_cast<uintptr_t>(FuncPtr) };
		Functions.emplace_back(Data);
	}

	vec<InternalPropertyData>* GetPropertiesVectorPointer()
	{
		return &Properties;
	}

	vec<InternalFunctionData>* GetFunctionsVectorPointer()
	{
		return &Functions;
	}

	vec<InternalFunctionData> GetFunctions()
	{
		return Functions;
	}

	vec<InternalPropertyData> GetProperties()
	{
		return Properties;
	}

	template<typename PropertyType>
	PropertyType GetPropertyValue(const std::string& PropName)
	{
		for (InternalPropertyData& PropObj : GetProperties())
		{
			if (PropObj.Name == PropName)
			{
				auto Val = reinterpret_cast<PropertyType*>(PropObj.ValuePointer);
				return *Val;
			}
		}
		return PropertyType();
	}

	template<typename ReturnType, typename FunctionType>
	ReturnType CallFunction(const std::string& FuncName)
	{
		for (const InternalFunctionData& FuncObj : GetFunctions())
		{
			if (FuncObj.Name == FuncName)
			{
				auto Func = reinterpret_cast<std::function<FunctionType>*>(FuncObj.FunctionPointer);
				ReturnType ReturnedVal = (*Func)();
				return ReturnedVal;
			}
		}

		return ReturnType();
	}
};
