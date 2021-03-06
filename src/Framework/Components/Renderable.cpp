#include "GM/Framework/Components/Camera.h"
#include "GM/Framework/Components/Renderable.h"
#include "GM/Framework/DefinitionsPropertyNames.h"
#include "GM/Framework/Entity.h"
#include "GM/Framework/Managers/MaterialManager.h"
#include "GM/Framework/Managers/MeshManager.h"
#include "GM/Framework/Systems/RenderSystem.h"
#include "GM/Framework/Utilities/Material.h"

#include "GM/Core/GL/Render.h"
#include "GM/Core/GL/Shader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

namespace GM {
namespace Framework {

Renderable::Renderable(const EntityPtr &owner, const RenderSystemPtr &render_system, const MaterialManagerPtr material_manager, const MeshManagerPtr mesh_manager, const unsigned int render_layers, const std::string &name)
: Component<Renderable>(owner, name)
, render_system(render_system)
, material_manager(material_manager)
, mesh_manager(mesh_manager)
, render_layers(render_layers)
{
	render_system->add_renderable(this);

	world_matrix_property = owner->add(GM_PROPERTY_WORLD_MATRIX, glm::mat4(1));
	object_matrix_property = owner->add(GM_PROPERTY_OBJECT_MATRIX, glm::mat4(1));
	normal_matrix_property = owner->add(GM_PROPERTY_NORMAL_MATRIX, glm::mat3(1));

	culled_property = owner->add(GM_PROPERTY_CULLED, false);
	visible_property = owner->add(GM_PROPERTY_VISIBLE, false);

	material_name_property = owner->add(GM_PROPERTY_MATERIAL_NAME, std::string());
	slots.connect(material_name_property.value_changed(),
		[this](const std::string &/*old_material_name*/, const std::string &new_material_name)
		{
			uniforms_for_render_pass.clear();
			this->new_uniform_listener_slot = clan::Slot();
			if (new_material_name.empty())
			{
				this->material = nullptr;
			}
			else
			{
				this->material = this->material_manager->get_or_create(new_material_name);
				this->set_up_uniform_listeners();
				if (this->material->has_property(GM_PROPERTY_MESH_PIVOT_POINT_OFFSET)) {
					this->material->get<glm::vec3>(GM_PROPERTY_MESH_PIVOT_POINT_OFFSET) = mesh_pivot_point_offset_property.get();
				}
			}
		});

	mesh_name_property = owner->add(GM_PROPERTY_MESH_NAME, std::string());
	slots.connect(mesh_name_property.value_changed(),
		[this](const std::string &/*old_mesh_name*/, const std::string &new_mesh_name) mutable
		{
			if (new_mesh_name.empty())
			{
				mesh = nullptr;
			}
			else
			{
				mesh = this->mesh_manager->get_or_create(new_mesh_name);
			}
		});

	mesh_pivot_point_offset_property = owner->add(GM_PROPERTY_MESH_PIVOT_POINT_OFFSET, glm::vec3());
	slots.connect(mesh_pivot_point_offset_property.value_changed(), 
		[this](const glm::vec3 &/*old_value*/, const glm::vec3 &new_value) mutable 
		{
			if (this->material != nullptr) {
				if (this->material->has_property(GM_PROPERTY_MESH_PIVOT_POINT_OFFSET)) {
					this->material->get<glm::vec3>(GM_PROPERTY_MESH_PIVOT_POINT_OFFSET) = new_value;
				}
			}
		});
}

Renderable::~Renderable()
{
	render_system->remove_renderable(this);
}

void Renderable::set_up_uniform_listeners()
{
	uniforms_for_render_pass.clear();

	for (const auto &pair : owner->get_properties())
	{
		add_uniform_listener(pair.second);
	}

	new_uniform_listener_slot = owner->property_added().connect(this, &Renderable::add_uniform_listener);
}

void Renderable::add_uniform_listener(const std::shared_ptr<IProperty> &prop)
{
	for (const auto &map_entry : material->get_render_pass_configs())
	{
		const std::string &render_pass_name = map_entry.first;
		const Material::RenderPassConfig &settings = map_entry.second;

		const std::vector<std::string> &unused_uniforms = settings.unused_uniforms;

		if (std::find(unused_uniforms.begin(), unused_uniforms.end(), prop->get_name()) == unused_uniforms.end())
		{
			continue;
		}

		const Core::ShaderPtr &shader = settings.shader;
		const Core::ShaderVariableInfo info = shader->get_uniform_info(prop->get_name());

		switch (info.type)
		{
		case GL_BOOL:
			{
				add_uniform<bool>(render_pass_name, prop, shader->get_handle(), info.location);
				break;
			}

		case GL_BYTE:
			{
				add_uniform<char>(render_pass_name, prop, shader->get_handle(), info.location);
				break;
			}

		case GL_UNSIGNED_BYTE:
			{
				add_uniform<unsigned char>(render_pass_name, prop, shader->get_handle(), info.location);
				break;
			}

		case GL_SHORT:
			{
				add_uniform<short>(render_pass_name, prop, shader->get_handle(), info.location);
				break;
			}

		case GL_UNSIGNED_SHORT:
			{
				add_uniform<unsigned short>(render_pass_name, prop, shader->get_handle(), info.location);
				break;
			}

		case GL_INT:
			{
				add_uniform<int>(render_pass_name, prop, shader->get_handle(), info.location);
				break;
			}

		case GL_UNSIGNED_INT:
			{
				add_uniform<unsigned int>(render_pass_name, prop, shader->get_handle(), info.location);
				break;
			}

		case GL_FLOAT:
			{
				add_uniform<float>(render_pass_name, prop, shader->get_handle(), info.location);
				break;
			}

		case GL_FLOAT_VEC2:
			{
				add_uniform<glm::vec2>(render_pass_name, prop, shader->get_handle(), info.location);
				break;
			}

		case GL_INT_VEC2:
			{
				add_uniform<glm::ivec2>(render_pass_name, prop, shader->get_handle(), info.location);
				break;
			}

		case GL_UNSIGNED_INT_VEC2:
			{
				add_uniform<glm::uvec2>(render_pass_name, prop, shader->get_handle(), info.location);
				break;
			}

		case GL_FLOAT_VEC3:
			{
				add_uniform<glm::vec3>(render_pass_name, prop, shader->get_handle(), info.location);
				break;
			}

		case GL_INT_VEC3:
			{
				add_uniform<glm::ivec3>(render_pass_name, prop, shader->get_handle(), info.location);
				break;
			}

		case GL_UNSIGNED_INT_VEC3:
			{
				add_uniform<glm::uvec3>(render_pass_name, prop, shader->get_handle(), info.location);
				break;
			}

		case GL_FLOAT_VEC4:
			{
				add_uniform<glm::vec4>(render_pass_name, prop, shader->get_handle(), info.location);
				break;
			}

		case GL_INT_VEC4:
			{
				add_uniform<glm::ivec4>(render_pass_name, prop, shader->get_handle(), info.location);
				break;
			}

		case GL_UNSIGNED_INT_VEC4:
			{
				add_uniform<glm::uvec4>(render_pass_name, prop, shader->get_handle(), info.location);
				break;
			}

		case GL_FLOAT_MAT2:
			{
				add_uniform<glm::mat2>(render_pass_name, prop, shader->get_handle(), info.location);
				break;
			}

		case GL_FLOAT_MAT3:
			{
				add_uniform<glm::mat3>(render_pass_name, prop, shader->get_handle(), info.location);
				break;
			}

		case GL_FLOAT_MAT4:
			{
				add_uniform<glm::mat4>(render_pass_name, prop, shader->get_handle(), info.location);
				break;
			}

		case GL_SAMPLER_2D:
		case GL_SAMPLER_CUBE:
			{
				// FIXME: get texture units somehow
				add_uniform<Core::TexturePtr>(render_pass_name, prop, shader->get_handle(), info.location);
				break;
			}

		default:
			{
				clan::StringFormat message("Uknown/unhandled type (%1) for uniform (%2) with length (%3)");
				message.set_arg(1, info.type);
				message.set_arg(2, info.name);
				message.set_arg(3, info.size);

				std::cerr << message.get_result();
				break;
			}
		}
	}
}

void Renderable::update_uniforms(Camera *camera, const std::string &render_pass_name) { 
	if (material->has_property(GM_PROPERTY_PROJECTION_MATRIX)) {
		material->get<glm::mat4>(GM_PROPERTY_PROJECTION_MATRIX) = camera->get_projection_matrix();
	}

	if (material->has_property(GM_PROPERTY_VIEW_MATRIX)) {
		material->get<glm::mat4>(GM_PROPERTY_VIEW_MATRIX) = camera->get_view_matrix();
	}

	// Let's only actually update the normal matrix value based on world and view matrix if the shader use it.
	if (material->has_property(GM_PROPERTY_NORMAL_MATRIX)) {
		update_normal_matrix(camera->get_view_matrix());
	}

	uniforms_for_render_pass[render_pass_name].update_uniforms_signal(); 
}

void Renderable::update_normal_matrix(const glm::mat4 &view_matrix) {
	normal_matrix_property = make_normal_matrix(view_matrix);
}

glm::mat3 Renderable::make_normal_matrix(const glm::mat4 &view_matrix) const {
	return glm::inverseTranspose(glm::mat3(view_matrix * world_matrix_property.get()));
}

} // namespace Framework
} // namespace GM
