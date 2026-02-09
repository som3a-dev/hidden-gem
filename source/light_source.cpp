#include "light_source.h"

#include <string.h>
#include <stdio.h>

static const int light_count = 10;

void LightSource::set_uniforms(Shader& lighting_shader, int shader_index) const
{
    if ((shader_index < 0) || (shader_index >= light_count))
    {
        TraceLog(LOG_WARNING, "Invalid shader index passed to light source");
        return;
    }

    Vector2 pos = {x, y};
    char buf[64];
    size_t bufsz = sizeof(buf) / sizeof(char);

    snprintf(buf, bufsz, "light[%d].pos", shader_index);
    int pos_uniform = GetShaderLocation(lighting_shader, buf);
    SetShaderValue(lighting_shader, pos_uniform, &pos, SHADER_UNIFORM_VEC2);

    snprintf(buf, bufsz, "light[%d].radius", shader_index);
    int radius_uniform = GetShaderLocation(lighting_shader, buf);
    SetShaderValue(lighting_shader, radius_uniform, &radius, SHADER_UNIFORM_FLOAT);

    snprintf(buf, bufsz, "light[%d].height", shader_index);
    int height_uniform = GetShaderLocation(lighting_shader, buf);
    SetShaderValue(lighting_shader, height_uniform, &height, SHADER_UNIFORM_FLOAT);

    snprintf(buf, bufsz, "light[%d].color", shader_index);
    int color_uniform = GetShaderLocation(lighting_shader, buf);
    SetShaderValue(lighting_shader, color_uniform, &color, SHADER_UNIFORM_VEC3);
}
