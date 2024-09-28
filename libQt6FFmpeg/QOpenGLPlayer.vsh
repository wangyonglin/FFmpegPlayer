#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

attribute vec4 vertex;
attribute vec2 texCoord;
varying vec2 texc;
void main(void)
{
    gl_Position = vertex;
    texc = texCoord;
}
