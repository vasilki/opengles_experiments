#version 100
attribute vec3 position;
uniform vec2 screensize;
void main() {
  vec3 ndc;

  ndc.x = 2.0 * position.x / screensize.x - 1.0;
  ndc.y = 1.0 - 2.0 * position.y / screensize.y;
  ndc.z = 0.0;

  gl_Position = vec4(ndc, 1.0);
}
