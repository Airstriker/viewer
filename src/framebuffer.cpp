#include "framebuffer.hpp"

#include "opengl_state.hpp"

Framebuffer::Framebuffer(int width, int height) :
  m_width(width),
  m_height(height),
  m_fbo(0),
  m_color_buffer(),
  m_depth_buffer()
{
  log_info("Framebuffer(%d, %d)", width, height);
  OpenGLState state;

  // create the framebuffer
  assert_gl("framebuffer");
  glGenFramebuffers(1, &m_fbo);
  assert_gl("framebuffer");
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
  assert_gl("framebuffer");

  m_color_buffer = Texture::create_empty(GL_TEXTURE_2D, GL_RGB16F, width, height);
  m_depth_buffer = Texture::create_shadowmap(width, height);
    
  // attach color and depth buffer
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_color_buffer->get_target(), m_color_buffer->get_id(), 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  m_depth_buffer->get_target(), m_depth_buffer->get_id(), 0);
  assert_gl("framebuffer");

  GLenum complete = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (complete != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "Framebuffer incomplete: " << complete << std::endl;
  }
  assert_gl("framebuffer");

  std::cout << "FBO: " << m_fbo << std::endl;
  std::cout << "Depth Buffer: " << m_depth_buffer << std::endl;
  std::cout << "Color Buffer: " << m_color_buffer << std::endl;

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
  
Framebuffer::~Framebuffer()
{
  assert_gl("~Framebuffer-enter()");
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  log_info("~Framebuffer()");
  glDeleteFramebuffers(1, &m_fbo);
  //glDeleteTextures(1, &m_depth_buffer);
  //glDeleteTextures(1, &m_color_buffer);
  assert_gl("~Framebuffer()");
}

void
Framebuffer::draw(float x, float y, float w, float h, float z)
{
  OpenGLState state;
    
  glEnable(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, m_color_buffer->get_id());
  glBegin(GL_QUADS);
  {
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(x, y, z);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(x+w, y, z);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(x+w, y+h, z); // FIXME

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(x, y+h, z);
  }
  glEnd();  
}

void
Framebuffer::draw_depth(float x, float y, float w, float h, float z)
{
  assert(!"Never touch");

  OpenGLState state;
    
  glEnable(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, m_depth_buffer->get_id());

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  //glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE); 

  glBegin(GL_QUADS);
  {
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(x, y, z);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(x+w, y, z);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(x+w, y+h, z); // FIXME

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(x, y+h, z);
  }
  glEnd();  
}

void
Framebuffer::bind()
{
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void 
Framebuffer::unbind()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/* EOF */
