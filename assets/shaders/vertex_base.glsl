#version 100
attribute vec3 position;
uniform mat3 transform;
void main() {
  vec3 loc_out;
  loc_out = transform * position;
  gl_Position = vec4(loc_out, 1.0);
}
