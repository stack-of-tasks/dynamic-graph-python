/* Copyright 2020 LAAS, CNRS
 * Joseph Mirabel
 *
 */

#define ENABLE_RT_LOG

#include <dynamic-graph/entity.h>
#include <dynamic-graph/signal-ptr.h>
#include <dynamic-graph/signal-time-dependent.h>

#include <sstream>

namespace dynamicgraph {
class CustomEntity : public Entity {
 public:
  dynamicgraph::SignalPtr<double, sigtime_t> m_sigdSIN;
  dynamicgraph::SignalTimeDependent<double, sigtime_t> m_sigdTimeDepSOUT;

  DYNAMIC_GRAPH_ENTITY_DECL();
  CustomEntity(const std::string n);

  void addSignal();

  void rmValidSignal();

  double &update(double &res, const sigtime_t &inTime);

  void act();
};
}  // namespace dynamicgraph
