#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

in vec2 worldPos;

uniform vec2 lightPos;
uniform float lightRadius;
uniform vec3 lightColor;

uniform float ambientAttenuation;

uniform sampler2D texture0;
uniform sampler2D normalMap;

const float lightHeight = 100.0;

void basicFallOff()
{
    float lightDistance = length(lightPos - worldPos);
    float att = smoothstep(lightRadius, ambientAttenuation, lightDistance);
//    float att = max(ambientAttenuation, 1.0 - (lightDistance / lightRadius));

    finalColor = texture(texture0, fragTexCoord) * vec4(lightColor * att, 1.0); 
}

void normalTest()
{
    vec3 T = vec3(1.0, 0.0, 0.0);
    vec3 B = vec3(0.0, 1.0, 0.0);
    vec3 N = vec3(0.0, 0.0, 1.0);

    mat3 TBN = mat3(T, B, N);

    vec4 albedo = texture(texture0, fragTexCoord);

    vec3 normal = texture(normalMap, fragTexCoord).rgb;
    normal = normal * 2.0 - 1.0;
    normal = normalize(normal);

    vec3 lightDir = normalize(vec3(lightPos - worldPos, lightHeight));

    float lightDistance = length(lightPos - worldPos);

    float att = mix(1.0, 0.0,
                smoothstep(0.0, lightRadius, lightDistance));
//    float att = max(ambientAttenuation, 1.0 - (lightDistance / lightRadius));

    float diffuse = max(dot(normal, lightDir), 0.0);

    float lighting = max(ambientAttenuation, diffuse * att);

    finalColor = albedo * vec4((lightColor * lighting), 1.0);
}

void main()
{
    normalTest();
//    basicFallOff();
}