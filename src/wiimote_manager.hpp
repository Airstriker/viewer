#ifndef HEADER_WIIMOTE_MANAGER_HPP
#define HEADER_WIIMOTE_MANAGER_HPP

#include <wiicpp.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <thread>
#include <mutex>

class WiimoteManager
{
private:
  CWii m_cwii;
  bool m_reload_wiimotes;

  glm::vec3 m_smoothed_gravity;

  glm::vec3 m_accumulated;
  glm::quat m_gyro_orientation;
  glm::quat m_accel_orientation;
  glm::quat m_orientation;
  
  std::thread m_thread;
  mutable std::mutex m_mutex;
  
public:
  WiimoteManager();

  glm::quat get_gyro_orientation() const;
  glm::quat get_accel_orientation() const;
  glm::quat get_orientation() const;
  glm::quat get_accumulated() const;

  void reset_gyro_orientation(const glm::quat& value = glm::quat());

private:
  void update();
  void handle_event(CWiimote& wm);
};

#endif

/* EOF */
