#include "scene_manager.hpp"

#include "camera.hpp"
#include "render_context.hpp"

SceneManager::SceneManager() :
  m_world(new SceneNode),
  m_view(new SceneNode),
  m_lights(),
  m_override_material()
{}

SceneManager::~SceneManager()
{
}

SceneNode*
SceneManager::get_world() const
{
  return m_world.get();
}

SceneNode* 
SceneManager::get_view() const
{
  return m_view.get();
}

LightPtr
SceneManager::create_light() 
{
  LightPtr light = std::make_shared<Light>();
  m_lights.push_back(light);
  return light;
}

void
SceneManager::render(const Camera& camera, bool geometry_pass, Stereo stereo)
{
  m_world->update_transform();
  m_view->update_transform();

  render_node(camera, m_world.get(), geometry_pass, stereo);

  Camera id = camera;
  id.set_position(glm::vec3(0.0f, 0.0f, 0.0f));
  render_node(id, m_view.get(), geometry_pass, stereo);
}

void
SceneManager::render_node(const Camera& camera, SceneNode* node, bool geometry_pass, Stereo stereo)
{
  OpenGLState state;

  RenderContext context(camera, node);
  
  context.set_stereo(stereo);

  if (geometry_pass)
  {
    context.set_override_material(m_override_material);
  }

  for(auto& model : node->get_models())
  {
    model->draw(context);
  }

  for(const auto& child : node->get_children())
  {
    render_node(camera, child.get(), geometry_pass, stereo);
  }
}

void
SceneManager::set_override_material(MaterialPtr material)
{
  m_override_material = material;
}

/* EOF */
