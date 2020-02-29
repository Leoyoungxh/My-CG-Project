varying vec4 color;
varying vec2 texCoord;

uniform sampler2D texture;

void main()
{
	// gl_FragColor = vec4(1, 0, 0, 0 ) * texture2D( texture, texCoord );
    gl_FragColor = texture2D( texture, texCoord );
    // gl_FragColor = color;
    // gl_FragColor = vec4(0, 0, 0, 0 );
}