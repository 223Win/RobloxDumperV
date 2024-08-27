#pragma once
#include "BaseObject.h"
#include <unordered_map>
#include <string>
#include <functional>
#include <memory>
//
//// Base class for type-erased storage
//
//
//// TypeRegistry class for type registration and creation
//class TypeRegistry {
//public:
//    // Register a type with its name
//    template<typename T>
//    static void RegisterType(const std::string& typeName) {
//        typeMap[typeName] = []() -> std::unique_ptr<BaseType> {
//            return std::make_unique<TypeWrapper<T>>();
//            };
//    }
//
//    // Check if a type is supported
//    static bool IsTypeSupported(const std::string& typeName);
//
//    // Create an instance of the type with its name
//    static std::unique_ptr<BaseType> CreateInstance(const std::string& typeName);
//
//private:
//    // Wrapper class for type erasure
//    template<typename T>
//    class TypeWrapper : public BaseType {
//    public:
//        std::unique_ptr<BaseType> Clone() const override;
//    };
//
//    static std::unordered_map<std::string, std::function<std::unique_ptr<BaseType>()>> typeMap;
//};
//
//// Initialize the static member
////std::unordered_map<std::string, std::function<std::unique_ptr<BaseType>()>> TypeRegistry::typeMap;

template<typename T>
class TypeWrapper : public BaseType {
public:
    std::unique_ptr<BaseType> Clone() const override {
        return std::make_unique<TypeWrapper<T>>();
    }
};

class TypeRegistry
{
public:
	TypeRegistry();
    template<typename T>
    static void RegisterType(const std::string& typeName) {
        typeMap[typeName] = []() -> std::unique_ptr<BaseType> {
            return std::make_unique<TypeWrapper<T>>();
            };
    }
    static bool IsTypeSupported(const std::string& typeName);
    static std::unique_ptr<BaseType> CreateInstance(const std::string& typeName);

private:
    static std::unordered_map<std::string, std::function<std::unique_ptr<BaseType>()>> typeMap;
};
