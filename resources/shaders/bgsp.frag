uniform sampler2D texture;
uniform sampler2D colors;
uniform float colbank; //0-5

void main()
{
	vec4 t = texture2D(texture, gl_TexCoord[0].xy);
	
	float col = colbank / 6.0;
	
	float col2 = gl_Color.y * float(t.x == 0.0) + t.x;
	
	vec4 c = texture2D(colors, vec2(col2 + gl_Color.x, col));
	
	
	gl_FragColor = c;
}
