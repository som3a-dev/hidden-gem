#version 330

in vec3 vertexPosition;
in vec2 vertexTexCoord;

out vec2 fragTexCoord;

out vec2 worldPos;

uniform mat4 mvp; 

void main()
{
    worldPos = vertexPosition.xy;

    fragTexCoord = vertexTexCoord;
    gl_Position = mvp * vec4(vertexPosition, 1.0); 
}
