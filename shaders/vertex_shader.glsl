#version 110

varying vec3 normal;
out vec4 frag_color;
uniform float alpha;
uniform vec4 light_pos;
uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;

void main()
{
    normal = normalize(gl_NormalMatrix * gl_Normal);
		frag_color = gl_Color;
//		frag_color.a = alpha;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

