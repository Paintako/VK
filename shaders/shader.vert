#version 450

// location = 0 will be pointed in binding 0 in bindingdescriptor
layout (location = 0) in vec2 pos; 
layout (location = 1) in vec3 color;

// pass color to fragment shader, note that location = 0 does not equeal to input location
layout (location = 0) out vec3 fragColor;

void main() {
    gl_Position = vec4(pos, 0.0, 1.0);
    fragColor = color;
}