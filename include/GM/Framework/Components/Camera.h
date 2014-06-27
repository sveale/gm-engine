#pragma once

#include "../Totem/Component.h"
#include "../DefinitionsComponentNames.h"

#include <ClanLib/core.h>

#include <glm/glm.hpp>

#include <memory>

namespace GM {
namespace Framework {

class Camera; typedef std::shared_ptr<Camera> CameraPtr;
class Entity; typedef std::shared_ptr<Entity> EntityPtr;
class RenderSystem; typedef std::shared_ptr<RenderSystem> RenderSystemPtr;

class Camera : public Component<Camera> {
public:
	Camera(const EntityPtr &owner, const RenderSystemPtr &render_system, unsigned int render_layers = 1, unsigned int depth = 0, const std::string &name = std::string());
	virtual ~Camera();

	std::string get_type() const override { return get_static_type(); };

	unsigned int get_render_layers() const { return render_layers; };
	unsigned int get_depth() const { return depth; };

	bool is_view_matrix_dirty() const { return view_matrix_property.is_dirty(); };
	const glm::mat4 &get_view_matrix() const { return view_matrix_property; };

	bool is_projection_matrix_dirty() const { return projection_matrix_property.is_dirty(); };
	const glm::mat4 &get_projection_matrix() const { return projection_matrix_property; };

	void clear_dirty();

private:
	void recalculate_view_matrix(const glm::mat4 &old_world, const glm::mat4 &new_world);

	static std::string get_static_type() { return COMPONENT_CAMERA; };
private:
	clan::CallbackContainer slots;

	RenderSystemPtr render_system;

	// What render layer is this camera supposed to follow?
	unsigned int render_layers;

	// Depth determines the order in the render layer, deth 0 gets rendered first, and so on.
	unsigned int depth;

	Property<glm::mat4> projection_matrix_property;
	Property<glm::mat4> view_matrix_property;
	Property<glm::mat4> world_matrix_property;
};

} // namespace Framework
} // namespace GM
