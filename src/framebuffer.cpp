//  Simple 3D Model Viewer
//  Copyright (C) 2012-2013 Ingo Ruhnke <grumbel@gmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
    log_error("Framebuffer incomplete: %s", complete);
  }
  assert_gl("framebuffer");

  log_info("FBO: %s", m_fbo);
  log_info("Depth Buffer: %s", m_depth_buffer);
  log_info("Color Buffer: %s", m_color_buffer);

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
  OpenGLState state;
    
  glEnable(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, m_depth_buffer->get_id());
  
  GLint compare_mode;
  GLint compare_func;
  GLint depth_texture_mode;
  
  // capture old values
  glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, &compare_mode);
  glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, &compare_func);
  glGetTexParameteriv(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, &depth_texture_mode);

  // set new values
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE); 

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

  // restore old values
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, compare_mode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, compare_func);
  glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, depth_texture_mode);
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
