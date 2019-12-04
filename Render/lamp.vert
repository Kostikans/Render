attribute highp vec3 aVertex;
attribute highp vec2 aTexture;
attribute highp vec3 aNormal;
uniform highp mat4 modelMatrix;
uniform highp mat4 viewMatrix;
uniform highp mat4 projectionMatrix;


void main(void)
{
    gl_Position = projectionMatrix *  viewMatrix * modelMatrix * vec4(aVertex,1.0);
}
