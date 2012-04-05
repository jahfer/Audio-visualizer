uniform vec2 uAberrationOffset;

void main() {
	gl_TexCoord[0] = gl_MultiTexCoord0;

	vec4 v = vec4(gl_Vertex);

	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * v;
}