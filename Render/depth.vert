attribute highp vec3 aVertex;

uniform highp mat4 modelMatrix;

void main(void)
{
    gl_Position = modelMatrix * vec4(aVertex,1.0);
}
