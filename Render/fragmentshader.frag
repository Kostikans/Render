
uniform highp sampler2D texture;
uniform highp vec3 lightPos;
uniform highp vec3 viewPos;

varying highp vec2 texCoords;
varying highp vec3 fragPos;
varying highp vec3 normal;

void main()
{
    // ambient
    vec3 ambient = 0.15 * texture2D(texture, texCoords);


    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = 0.4 * diff * texture2D(texture, texCoords);

    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
    vec3 specular = 0.3 * spec * texture2D(texture, texCoords);


    // attenuation
    float distance    = length(lightPos - fragPos);
    float attenuation = 1.0 / (1.0 + 0.03 * distance + 0.010 * (distance * distance));

    ambient  *= attenuation;
    diffuse   *= attenuation;
    specular *= attenuation;

    vec3 result = ambient + diffuse + specular;
    gl_FragColor = vec4(result, 1.0);
}
