#version 330

// input variables, defaults from OpenFrameworks
in vec4 position;
in vec4 color;
in vec2 texcoord;

// MVP matrix
uniform mat4 modelViewProjectionMatrix;

uniform vec4 sphereLpos;
uniform vec4 sphereCpos;
uniform vec4 sphereRpos;

// output variables (to fragment shader)
out vec4 vs_Color; // pass through vertex color
out vec3 vs_distToSpheres; // distance of the vertex to the 3 spheres

void main(void) {
    // pass the position through modified by the transforms
    gl_Position = modelViewProjectionMatrix * position;

    // calculate the distance from the spheres
    vs_distToSpheres = vec3(sqrt(dot(sphereLpos, position)), 
                            sqrt(dot(sphereCpos, position)),
                            sqrt(dot(sphereRpos, position)));

    // output color to frag
    vs_Color = color;
}
