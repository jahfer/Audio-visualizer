// uniform variables are set once before drawing the whole scene and never change
// from vertex to vertex (or fragment to fragment)
uniform vec2 uNoiseOffset;
uniform vec3 uColor;
uniform float uAlpha;

void main() {
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
}