#ifndef HEADER_PROGRAM_HPP
#define HEADER_PROGRAM_HPP

#include <memory>
#include <string>
#include <GL/glew.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "assert_gl.hpp"
#include "log.hpp"
#include "shader.hpp"

class Program;

typedef std::shared_ptr<Program> ProgramPtr;

class Program
{
private:
  GLuint m_program;

public:
  static ProgramPtr create(ShaderPtr shader);
  static ProgramPtr create(ShaderPtr shader1, ShaderPtr shader2);
  static ProgramPtr create(ShaderPtr shader1, ShaderPtr shader2, ShaderPtr shader3);

public:
  Program();
  ~Program();

  void attach(ShaderPtr shader);
  void link();
  void validate();

  std::string get_info_log() const;
  bool get_link_status() const;
  bool get_validate_status() const;
  
  GLuint get_id() const { return m_program; }

  void inspect() const;

  template<typename T>
  void set_uniform(const std::string& name, const T& v)
  {
    assert_gl("set_uniform:enter");
    int loc = glGetUniformLocation(m_program, name.c_str());
    if (loc == -1)
    {
      //log_debug("uniform location '%s' not found, ignoring", name);
    }
    else
    {
      set_uniform(loc, v);
    }
    assert_gl("set_uniform:exit: %s", name);
  }
  
  void set_uniform(GLint loc, float v) { glProgramUniform1f(m_program, loc, v); }
  void set_uniform(GLint loc, const glm::vec2& v) { glProgramUniform2f(m_program, loc, v.x, v.y); }
  void set_uniform(GLint loc, const glm::vec3& v) { glProgramUniform3f(m_program, loc, v.x, v.y, v.z); }
  void set_uniform(GLint loc, const glm::vec4& v) { glProgramUniform4f(m_program, loc, v.x, v.y, v.z, v.w); }

  void set_uniform(GLint loc, int v) { glProgramUniform1i(m_program, loc, v); }
  void set_uniform(GLint loc, unsigned int v) { glProgramUniform1i(m_program, loc, v); }
  void set_uniform(GLint loc, const glm::ivec2& v) { glProgramUniform2i(m_program, loc, v.x, v.y); }
  void set_uniform(GLint loc, const glm::ivec3& v) { glProgramUniform3i(m_program, loc, v.x, v.y, v.z); }
  void set_uniform(GLint loc, const glm::ivec4& v) { glProgramUniform4i(m_program, loc, v.x, v.y, v.z, v.w); }

  void set_uniform(GLint loc, const glm::mat3& v) { glProgramUniformMatrix3fv(m_program, loc, 1, GL_FALSE, glm::value_ptr(v)); }
  void set_uniform(GLint loc, const glm::mat4& v) { glProgramUniformMatrix4fv(m_program, loc, 1, GL_FALSE, glm::value_ptr(v)); }

private:
  Program(const Program&);
  Program& operator=(const Program&);
};

#endif

/* EOF */
