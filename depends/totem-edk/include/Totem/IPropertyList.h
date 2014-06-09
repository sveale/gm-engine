#pragma once
#include <ClanLib/core.h>

 
 

namespace Totem
{

class IPropertyList
{
public:
	virtual const CL_String &getName() const = 0;
	virtual bool isNull() const = 0;
	virtual bool isDirty() const = 0;
	virtual void clearDirty() = 0;

	template<typename PropertyType>	static bool isType(const std::shared_ptr<IPropertyList> &property);
	template<typename PropertyType>	static bool isType(const IPropertyList &property);
	virtual unsigned int getRuntimeTypeId() const = 0;
	template<typename PropertyType>	static unsigned int getRuntimeTypeId();
};

#include "IPropertyList.inl"

} //namespace Totem

