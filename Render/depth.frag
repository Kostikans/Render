in  highp vec4 FragPos;

uniform  highp vec3 lightPos;
uniform  highp float far_plane;

void main()
{
    float lightDistance = length(FragPos.xyz - lightPos);

    lightDistance = lightDistance / far_plane;

    gl_FragDepth = lightDistance;
}
