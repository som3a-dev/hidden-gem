#ifndef _LIGHT_SOURCE
#define _LIGHT_SOURCE

#include "raylib.h"

struct LightColor 
{
    float r;
    float g;
    float b;
};

struct LightSource
{
    float x = 0;
    float y = 0;
    float radius = 0;
    float height = 0;
    float ambient_attenuation = 0;
    LightColor color = {0};

    void set_uniforms(Shader& lighting_shader) const;
};

#endif