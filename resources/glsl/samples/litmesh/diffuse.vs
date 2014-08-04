#version 330
#define POSITION 0
#define NORMAL 1
#define TEXCOORD 2

layout(location = POSITION) in vec3 position;
layout(location = NORMAL) in vec3 normal;
layout(location = TEXCOORD) in vec2 texcoord;

out vec4 position_in_viewspace;
out vec3 normal_in_viewspace;

uniform mat4 WorldMatrix;		// object_to_world
uniform mat4 ViewMatrix;		// world_to_view
uniform mat4 ProjectionMatrix;	// view_to_clip
uniform mat3 NormalMatrix;		// normal_to_view

void main() {
	position_in_viewspace = ViewMatrix * WorldMatrix * vec4(position, 1.0);
	normal_in_viewspace = NormalMatrix * normal;
<<<<<<< .mine
	gl_Position = ProjectionMatrix * position_in_viewspace;
}
=======
}

>>>>>>> .theirs
