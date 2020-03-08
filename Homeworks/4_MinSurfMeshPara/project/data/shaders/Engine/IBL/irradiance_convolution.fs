#version 330 core

#include "../../Math/sample.h"

out vec3 FragColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;

void main()
{
	// The world vector acts as the normal of a tangent surface
    // from the origin, aligned to WorldPos. Given this normal, calculate all
    // incoming radiance of the environment. The result of this radiance
    // is the radiance of light coming from -Normal direction, which is what
    // we use in the PBR shader to sample irradiance.
    vec3 N = normalize(WorldPos);

    vec3 irradiance = vec3(0.0);
	
	// TODO
    
    FragColor = irradiance;
}
