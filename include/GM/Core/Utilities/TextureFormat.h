#pragma once

#include "TextureParameter.h"

#include <string>
#include <vector>

namespace GM {
namespace Core {

/**
 * Texture format is just a container of parameters..
 * You should add specializations that derive from this container to make eg. 2D textures or cube maps
 */
class TextureFormat
{
public:
	virtual ~TextureFormat() {};
	
	unsigned int get_type() const { return type; };
	bool get_make_mipmap() const { return make_mipmap; };

	const std::vector<std::string> &get_image_files() const { return image_files; };

	const std::vector<TextureParameterInt> &get_int_parameters() const { return int_parameters; };
	const std::vector<TextureParameterFloat> &get_float_parameters() const { return float_parameters; };

	static int compare(const TextureFormat &current, const TextureFormat &other);

protected:
	// TODO: A bit unclear a bout these, why should they be protected?
	TextureFormat(const unsigned int type);

	void set_make_mipmap(bool value) { make_mipmap = value; };

	void add_image_file(const std::string &file_name) { image_files.push_back(file_name); };

	void set_parameter(unsigned int param_name, float param) { float_parameters.push_back(TextureParameterFloat(param_name, param)); };
	void set_parameter(unsigned int param_name, int param) { int_parameters.push_back(TextureParameterInt(param_name, param)); };

protected:
	const unsigned int type; // texture format type
	bool make_mipmap; // should we create a mipmap for the(se) texture(s)?

	// Can _not_ be sorted, order must be preserved
	std::vector<std::string> image_files;

	std::vector<TextureParameterInt> int_parameters;
	std::vector<TextureParameterFloat> float_parameters;
};

bool operator< (const TextureFormat &current, const TextureFormat &other);

// TODO: class or function?
class Texture2D : public TextureFormat
{
public:
	// FIXME: Set proper defines when GL is introdused
	Texture2D(const std::string &file_name) : TextureFormat(/*GL_TEXTURE_2D*/ 1)
	{
		add_image_file(file_name);

		// FIXME: Uncomment when GL is introduced
		//set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//set_parameter(GL_TEXTURE_WRAP_S, wrap_mode);
		//set_parameter(GL_TEXTURE_WRAP_T, wrap_mode);

		set_make_mipmap(true);
	};

	virtual ~Texture2D() {};
};

} // namespace Core
} // namespace GM
