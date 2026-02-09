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
    LightColor color = {0};

    /*
        shader_index : the index of the Light to set in the Light array in the shader
    */
    void set_uniforms(Shader& lighting_shader, int shader_index) const;
};

#endif