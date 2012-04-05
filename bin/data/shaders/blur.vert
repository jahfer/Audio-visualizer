const int MaxKernelSize = 25;
// array of offsets for accessing the base image
uniform vec2 uOffset[MaxKernelSize];
// size of kernel (width * height) for this execution
uniform float uKernelSize;
uniform float uScale;
// image to be convolved
uniform sampler2DRect baseTex;

void main() {
	gl_TexCoord[0] = gl_MultiTexCoord0;

	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
}