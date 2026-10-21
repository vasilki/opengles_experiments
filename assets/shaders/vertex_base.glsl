#version 100
attribute vec3 position;
attribute vec3 transform0;
attribute vec3 transform1;
attribute vec3 transform2;



void main() {
  vec3 loc_out;
  mat3 transform = mat3(
        transform0,
        transform1,
        transform2);

  loc_out = transform * position;
  gl_Position = vec4(loc_out, 1.0);
}
