#version 100
uniform mat3 ndc_matrix;

attribute vec3 position;
attribute float rotate_angle;
attribute float scale_x;
attribute float scale_y;
attribute float translate_x;
attribute float translate_y;
attribute vec4 color;



void main() {
  vec3 loc_out;
  mat3 transform = mat3(
        transform0,
        transform1,
        transform2);

  loc_out = transform * position;
  gl_Position = vec4(loc_out, 1.0);
}
