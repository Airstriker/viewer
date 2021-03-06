#include "shader.hpp"

#include <boost/format.hpp>
#include <fstream>

#include "log.hpp"

ShaderPtr
Shader::from_file(GLenum type, const std::string& filename)
{
  std::ifstream in(filename);
  if (!in)
  {
    throw std::runtime_error((boost::format("%s: failed to open file") % filename).str());
  }
  else
  {
    std::string source{std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()}; 
  
    //log_debug("shader source:\n %s\n", source);

    ShaderPtr shader = std::make_shared<Shader>(type);

    shader->source(source);
    shader->compile();

    if (!shader->get_compile_status())
    {
      throw std::runtime_error((boost::format("%s: error:\n %s") % filename % shader->get_info_log()).str());
    }

    log_debug("%s: shader compile successful", filename);
    return shader;
  }
}

Shader::Shader(GLenum type) :
  m_shader()
{
  m_shader = glCreateShader(type);
}

Shader::~Shader()
{
  glDeleteShader(m_shader);
}

void
Shader::source(const std::string& str)
{
  const char* source_lst[] = {str.c_str()};
  GLint length_lst[] = {static_cast<GLint>(str.size())};
  glShaderSource(m_shader, 1, source_lst, length_lst);
}

void
Shader::compile()
{
  glCompileShader(m_shader);
}

std::string
Shader::get_info_log() const
{
  GLint length;
  glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &length);

  if (length == 0)
  {
    return std::string();
  }
  else
  {
    GLsizei out_length;
    std::vector<char> str(length);
    glGetShaderInfoLog(m_shader, str.size(), &out_length, str.data());
    return std::string(str.begin(), str.end());
  }
}

bool
Shader::get_compile_status() const
{
  GLint compile_status;
  glGetShaderiv(m_shader, GL_COMPILE_STATUS, &compile_status);
  return compile_status == GL_TRUE;
}

/* EOF */
