
attribute highp vec3 aVertex;
attribute highp vec2 aTexture;
attribute highp vec3 aNormal;

uniform highp mat4 modelMatrix;
uniform highp mat4 viewMatrix;
uniform highp mat4 projectionMatrix;

varying highp vec2 texCoords;
varying highp vec3 normal;
varying highp vec3 fragPos;

void main(void)
{
    texCoords = aTexture;
    normal =  vec3(modelMatrix * vec4(aNormal, 0.0));
    fragPos = vec3(modelMatrix * vec4(aVertex, 1.0));
    gl_Position = projectionMatrix * viewMatrix * vec4(fragPos,1.0);

}
