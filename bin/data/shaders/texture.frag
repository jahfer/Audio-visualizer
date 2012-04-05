uniform sampler2DRect uInputTexture1;
uniform sampler2DRect uInputTexture2;
uniform vec2 uNoiseOffset;
uniform vec3 uColor;
uniform float uAlpha;

// Component wise blending
#define Blend(base, blend, funcf) 	vec3(funcf(base.r, blend.r), funcf(base.g, blend.g), funcf(base.b, blend.b))

#define BlendOverlayf(base, blend) 	(base < 0.5 ? (2.0 * base * blend) : (1.0 - 2.0 * (1.0 - base) * (1.0 - blend)))
#define BlendOverlay(base, blend) 	Blend(base, blend, BlendOverlayf)

void main() {

	vec3 inputColor = uColor / 255.;
	// noise texture
	vec4 col2 = texture2DRect(uInputTexture1, gl_TexCoord[0].xy + uNoiseOffset);
	col2.a = 0.3;

	// general texture
	vec4 col3 = texture2DRect(uInputTexture2, gl_TexCoord[0].xy);
	col3.a = 0.6;

	vec3 colFinal = inputColor * col3.rgb * 1.3;

	colFinal = mix(inputColor, colFinal, col3.a);
	//colFinal = uColor;

	// multiply screen by vignette
	//vec3 colFinal = uColor * col3.rgb;
	
	// overlay noise @ noise's opacity
	colFinal = mix(colFinal, BlendOverlay(colFinal, col2), col2.a);	
	
	float zShade = 1./gl_TexCoord[0].z;

	gl_FragColor = vec4(colFinal, uAlpha); // ghosting effect
}