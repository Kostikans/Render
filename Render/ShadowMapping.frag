
uniform highp sampler2D texture_diffuseN;
uniform highp samplerCube depthMap;
uniform highp vec3 lightPos;
uniform highp vec3 viewPos;
uniform float far_plane;

varying highp vec2 texCoords;
varying highp vec3 fragPos;
varying highp vec3 normal;

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1),
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec3 fragPos)
{
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = textureCube(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
    return shadow;
}

void main()
{

    // ambient
    vec3 ambient = 0.2 * texture2D(texture_diffuseN, texCoords);


    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = 0.5 * diff * texture2D(texture_diffuseN, texCoords);

    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
    vec3 specular = 0.4 * spec * texture2D(texture_diffuseN, texCoords);


    // attenuation
    float distance    = length(lightPos - fragPos);
    float attenuation = 1.0 / (1.0 + 0.03 * distance + 0.010 * (distance * distance));

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    float shadow = ShadowCalculation(fragPos);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * texture2D(texture_diffuseN, texCoords);
    gl_FragColor = vec4(lighting, 1.0);
}
