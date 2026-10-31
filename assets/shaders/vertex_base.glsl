#version 100
uniform mat3 ndc_matrix;

attribute vec2 position;
attribute float rotate_angle;
attribute vec2 scale;
attribute vec2 translate;
attribute vec4 color;

varying vec4 v_color;


void main() {
  float cos_a = cos(rotate_angle * 3.14159265 / 180.0);
  float sin_a = sin(rotate_angle * 3.14159265 / 180.0);
  mat2 mat_rotate = mat2(
     cos_a,  sin_a,
    -sin_a,  cos_a
  );
  mat2 mat_scale = mat2(
     scale.x,  0,
     0,  scale.y
  );

  mat3 mat_translate = mat3(
     1, 0, 0,
     0, 1, 0,
    translate.x, translate.y, 1
  );

  mat2 RS = mat_rotate * mat_scale;
  mat3 RS3x3 = mat3(
    RS[0][0], RS[0][1], 0.0,
    RS[1][0], RS[1][1], 0.0,
    0.0, 0.0 , 1.0
  );

  mat3 transform = mat_translate * RS3x3;

  transform = ndc_matrix * transform;

  vec3 loc_out = vec3(position, 1.0);
  loc_out = transform * loc_out;

  v_color = color;
  gl_Position = vec4(loc_out, 1.0);
}



