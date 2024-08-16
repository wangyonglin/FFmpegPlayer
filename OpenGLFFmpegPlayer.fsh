#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform sampler2D texture;
varying vec2 texc;
void main(void)
{
    gl_FragColor = texture2D(texture,texc);
}
