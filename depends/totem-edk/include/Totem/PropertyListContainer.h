#pragma once
#include <ClanLib/core.h>

#include "PropertyList.h"

 
#include <unordered_map>
#include <vector>
 

namespace Totem
{

class PropertyListFactory
{
public:
	template<class PropertyType> static std::shared_ptr<Totem::PropertyList<PropertyType>> createPropertyList(const CL_String &name);
};

template<class UserData = void*>
class PropertyListContainer
{
public:
	PropertyListContainer();
	virtual ~PropertyListContainer();

	bool hasPropertyList(const CL_String& name);

	template<class T> PropertyList<T> addList(const CL_String& name);
	template<class T> PropertyList<T> addList(const CL_String& name, const UserData &userData);
	void addList(std::shared_ptr<IPropertyList> property);

	template<class T> PropertyList<T> getList(const CL_String& name);
	std::shared_ptr<IPropertyList> getListInterface(const CL_String& name);
	std::unordered_map<CL_String, std::shared_ptr<IPropertyList>> &getPropertyLists();

	void removePropertyList(const CL_String& name, bool postponeDelete = false);
	void removePropertyList(const CL_String& name, const UserData &userData, bool postponeDelete = false);
	void removeAllPropertyLists();

	void clearDeletedPropertyLists();
	void clearDirtyPropertyLists();

	void updatePropertyLists();

	PropertyListContainer &operator= (const PropertyListContainer &rhs);

	CL_Signal_v<std::shared_ptr<IPropertyList>> &propertyListAdded();
	CL_Signal_v<std::shared_ptr<IPropertyList>, const UserData&> &propertyListWithUserDataAdded();
	CL_Signal_v<std::shared_ptr<IPropertyList>> &propertyListRemoved();
	CL_Signal_v<std::shared_ptr<IPropertyList>, const UserData&> &propertyListWithUserDataRemoved();

protected:
	std::unordered_map<CL_String, std::shared_ptr<IPropertyList>> propertyLists;
	std::vector<std::shared_ptr<IPropertyList>> deletedPropertyLists;

	CL_Signal_v<std::shared_ptr<IPropertyList>> sign_PropertyListAdded;
	CL_Signal_v<std::shared_ptr<IPropertyList>, const UserData&> sign_PropertyListWithUserDataAdded;
	CL_Signal_v<std::shared_ptr<IPropertyList>> sign_PropertyListRemoved;
	CL_Signal_v<std::shared_ptr<IPropertyList>, const UserData&> sign_PropertyListWithUserDataRemoved;
};

#include "PropertyListContainer.inl"

} //namespace Totem 
