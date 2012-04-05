uniform sampler2DRect baseTex;
uniform vec2 uAberrationOffset;

void main() {
		
	// 	v.z = sin(waveWidth * v.x + waveTime) * cos(waveWidth * v.y + waveTime) * waveHeight;
	vec4 coords = gl_TexCoord[0];

	// FBO of screen
	vec4 fbo1 = texture2DRect(baseTex, coords.xy - uAberrationOffset);	
	vec4 fbo2 = texture2DRect(baseTex, coords.xy);	
	vec4 fbo3 = texture2DRect(baseTex, coords.xy + uAberrationOffset);	
	
	// FBO channels mixed (incl. offsets)
	vec4 colFinal = vec4(fbo1.r, fbo2.g, fbo3.b, 1.);
	
	// Output final pixel color
	gl_FragColor = colFinal;
}