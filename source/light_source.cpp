#include "light_source.h"

void LightSource::set_uniforms(Shader& lighting_shader) const
{
    Vector2 pos = {x, y};

    int light_pos_uniform = GetShaderLocation(lighting_shader, "lightPos");
    SetShaderValue(lighting_shader, light_pos_uniform, &pos, SHADER_UNIFORM_VEC2);

    int light_radius_uniform = GetShaderLocation(lighting_shader, "lightRadius");
    SetShaderValue(lighting_shader, light_radius_uniform, &radius, SHADER_UNIFORM_FLOAT);

    int light_height_uniform = GetShaderLocation(lighting_shader, "lightHeight");
    SetShaderValue(lighting_shader, light_height_uniform, &height, SHADER_UNIFORM_FLOAT);

    int light_color_uniform = GetShaderLocation(lighting_shader, "lightColor");
    SetShaderValue(lighting_shader, light_color_uniform, &color, SHADER_UNIFORM_VEC3);

    int ambient_uniform = GetShaderLocation(lighting_shader, "ambientAttenuation");
    SetShaderValue(lighting_shader, ambient_uniform, &ambient_attenuation, SHADER_UNIFORM_FLOAT);
}
