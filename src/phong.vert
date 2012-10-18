#version 420 core
// ---------------------------------------------------------------------------
in vec3 position;
in vec3 normal;

out vec3 frag_normal;
out vec3 frag_position;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;
// ---------------------------------------------------------------------------
uniform mat4 ShadowMapMatrix;
out vec4 shadow_position;
// ---------------------------------------------------------------------------
void main(void)
{
  shadow_position = ShadowMapMatrix * vec4(position, 1.0);

  frag_position = vec3(ModelViewMatrix * vec4(position, 1.0));
  frag_normal = NormalMatrix * normal;

  gl_Position = MVP * vec4(position, 1.0);
}

/* EOF */
