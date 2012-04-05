const int MaxKernelSize = 25;

// array of offsets for accessing the base image
uniform vec2 uOffset[MaxKernelSize];
// size of kernel (width * height) for this execution
uniform float uKernelSize;
uniform float uScale;
// image to be convolved
uniform sampler2DRect baseTex;

void main() {
	vec4 sum = vec4(0.);
	
	/*for (int i=0; i<uKernelSize; i++) {
		sum += texture2DRect(baseTex, gl_TexCoord[0].xy + uOffset[i]);
	}*/
	
	sum += texture2DRect(baseTex, gl_TexCoord[0].xy + vec2(-1, -1));
	sum += texture2DRect(baseTex, gl_TexCoord[0].xy + vec2( 0, -1));
	sum += texture2DRect(baseTex, gl_TexCoord[0].xy + vec2( 1, -1));
	
	sum += texture2DRect(baseTex, gl_TexCoord[0].xy + vec2(-1,  0));
	sum += texture2DRect(baseTex, gl_TexCoord[0].xy + vec2( 0,  0));
	sum += texture2DRect(baseTex, gl_TexCoord[0].xy + vec2( 1,  0));
	
	sum += texture2DRect(baseTex, gl_TexCoord[0].xy + vec2(-1,  1));
	sum += texture2DRect(baseTex, gl_TexCoord[0].xy + vec2( 0,  1));
	sum += texture2DRect(baseTex, gl_TexCoord[0].xy + vec2( 1,  1));
	
	gl_FragColor = sum * vec4(uScale);
}