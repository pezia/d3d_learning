#pragma once

#include <string>

class IProperty
{
public:
	virtual std::string getName() = 0;
	virtual void* getValue() = 0;
};

