#version 420 core

in vec3  position;
in vec3  point_size;
in float alpha;

out float frag_alpha;

uniform mat4 ModelViewMatrix;
uniform mat4 MVP;

void main(void)
{
  vec4 pos = ModelViewMatrix * vec4(position, 1.0);
  gl_Position = MVP * vec4(position, 1.0);
  gl_PointSize = point_size * abs(1.0/pos.z);
  frag_alpha = alpha;
}

/* EOF */
