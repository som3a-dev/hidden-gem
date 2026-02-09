#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

in vec2 worldPos;

struct Light
{
    vec2 pos;
    float radius;
    float height;
    vec3 color;
};


#define MAX_LIGHTS 10
// If this array's size is updated, it must be updated in the game code too (LightSource)
uniform Light light[MAX_LIGHTS];

uniform float ambientAtt;

uniform sampler2D texture0;
uniform sampler2D normalMap;

void basicFallOff()
{
    float lightDistance = length(light[0].pos - worldPos);
    float att = smoothstep(light[0].radius, ambientAtt, lightDistance);
//    float att = max(ambientAttenuation, 1.0 - (lightDistance / lightRadius));

    finalColor = texture(texture0, fragTexCoord) * vec4(light[0].color * att, 1.0); 
}

vec3 normalMapping(int i)
{
    vec3 T = vec3(1.0, 0.0, 0.0);
    vec3 B = vec3(0.0, 1.0, 0.0);
    vec3 N = vec3(0.0, 0.0, 1.0);
    mat3 TBN = mat3(T, B, N);

    vec3 normal = texture(normalMap, fragTexCoord).rgb;
    normal = normal * 2.0 - 1.0;
    normal = normalize(normal);

    vec3 lightDir = normalize(vec3(light[i].pos - worldPos, light[i].height));
    float lightDistance = length(light[i].pos - worldPos);

    float att = mix(1.0, 0.0,
                smoothstep(0.0, light[i].radius, lightDistance));
//    float att = max(ambientAttenuation, 1.0 - (lightDistance / lightRadius));
    float diffuse = max(dot(normal, lightDir), 0.0);

//    float lighting = max(light[0].ambientAtt, diffuse * att);
//    vec4 color = albedo * vec4((light[0].color * lighting), 1.0);

    return light[i].color * diffuse * att;

/*    finalColor.r = (color.r + finalColor.r) / 2;
    finalColor.g = (color.g + finalColor.g) / 2;
    finalColor.b = (color.b + finalColor.b) / 2;
    finalColor.a = (color.a + finalColor.a) / 2;*/
}

void main()
{
    vec3 lighting = vec3(0.0);
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        lighting += normalMapping(i);
    }

    vec3 ambient = vec3(ambientAtt);

    vec4 albedo = texture(texture0, fragTexCoord);
    finalColor.rgb = albedo.rgb * (ambient + lighting);
    finalColor.a = albedo.a;
}