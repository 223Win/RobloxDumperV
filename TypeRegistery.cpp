#pragma once
#include "TypeRegistery.h"
#include "BaseObject.h"
#include <unordered_map>
#include <string>
#include <functional>
#include <memory>
#include "BaseObject.h"

std::unordered_map<std::string, std::function<std::unique_ptr<BaseType>()>> TypeRegistry::typeMap;

TypeRegistry::TypeRegistry()
{
}

bool TypeRegistry::IsTypeSupported(const std::string& typeName) {
    return typeMap.find(typeName) != typeMap.end();
}

std::unique_ptr<BaseType> TypeRegistry::CreateInstance(const std::string& typeName) {
    auto it = typeMap.find(typeName);
    if (it != typeMap.end()) {
        return it->second();
    }
    return nullptr;
}

