// shadowmap
uniform mat4 ShadowMapMatrix;
varying vec4 ProjShadow;
varying vec4 position;

// phong
varying vec3 normal;
varying vec3 eyeVec;
varying vec3 lightDir[2];

// normal mapping
varying vec3 lightDir_;
varying vec3 viewDir;

void main(void)
{
  { // shadowmap
    ProjShadow  = ShadowMapMatrix * gl_Vertex;
  }

  { // phong
    normal = gl_NormalMatrix * gl_Normal;

    vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);

    for(int i = 0; i < 2; ++i)
    {
      lightDir[i] = vec3(gl_LightSource[i].position.xyz - vVertex);
    }

    eyeVec = -vVertex;
  }

  { // normal mapping 
    vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);

    vec3 n = normalize(gl_NormalMatrix * gl_Normal);
    vec3 t = normalize(gl_NormalMatrix * gl_MultiTexCoord1.xyz);
    vec3 b = cross(n, t) * gl_MultiTexCoord1.w;
    
    mat3 tbnMatrix = mat3(t.x, b.x, n.x,
                          t.y, b.y, n.y,
                          t.z, b.z, n.z);

    lightDir_ = (gl_LightSource[0].position.xyz - vVertex);
    lightDir_ = tbnMatrix * lightDir_;

    viewDir = -vVertex;
    viewDir = tbnMatrix * viewDir;
  }

  { // regular stuff
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
    gl_Position = position = ftransform();
  }
}

/* EOF */
