#pragma once

#include <Totem/Totem.h>
#include <string>

namespace GM {
namespace Framework {

class Transform; typedef std::shared_ptr<Transform> TransformPtr;

class Entity : public Totem::PropertyContainer<>, public Totem::ComponentContainer<> {
public:
	Entity(const std::string &name);
	virtual ~Entity();

	std::string get_name() const { return name; };

	TransformPtr get_transform(std::function<void(const TransformPtr &)> func = nullptr) const;
	bool has_transform() const { return transform != nullptr; }

private:
	void on_component_added(std::shared_ptr<Totem::IComponent<>> component);
	void on_component_removed(std::shared_ptr<Totem::IComponent<>> component);

private:
	std::string name;

	// It's needed to save connected callbacks in order to disconnect them
	clan::Callback<void(std::shared_ptr<Totem::IComponent<>>)> component_removed_slot;

	TransformPtr transform;
};

} // namespace Framework
} // namespace GM
