/* Copyright 2010-2019 LAAS, CNRS
 * Thomas Moulard.
 *
 */

#define ENABLE_RT_LOG

#include <sstream>
#include <dynamic-graph/entity.h>
#include <dynamic-graph/exception-factory.h>
#include <dynamic-graph/factory.h>
#include <dynamic-graph/pool.h>
#include <dynamic-graph/real-time-logger.h>
#include <dynamic-graph/signal-ptr.h>
#include <dynamic-graph/signal-time-dependent.h>
#include <dynamic-graph/command-bind.h>

namespace dynamicgraph {
class CustomEntity : public Entity {
 public:
  dynamicgraph::SignalPtr<double, int> m_sigdSIN;
  dynamicgraph::SignalTimeDependent<double, int> m_sigdTimeDepSOUT;

  static const std::string CLASS_NAME;
  virtual const std::string &getClassName() const { return CLASS_NAME; }
  CustomEntity(const std::string n)
      : Entity(n),
        m_sigdSIN(NULL, "CustomEntity(" + name + ")::input(double)::in_double"),
        m_sigdTimeDepSOUT(boost::bind(&CustomEntity::update, this, _1, _2), m_sigdSIN,
                          "CustomEntity(" + name + ")::input(double)::out_double")

  {
    addSignal();

    using namespace dynamicgraph::command;

    this->addCommand("act", makeCommandVoid0( *this, &CustomEntity::act,
          docCommandVoid0( "act on input signal")));
  }

  void addSignal() { signalRegistration(m_sigdSIN << m_sigdTimeDepSOUT); }

  void rmValidSignal() {
    signalDeregistration("in_double");
    signalDeregistration("out_double");
  }

  double &update(double &res, const int &inTime) {
    const double &aDouble = m_sigdSIN(inTime);
    res = aDouble;
    logger().stream(MSG_TYPE_ERROR) << "start update " << res << '\n';
    DYNAMIC_GRAPH_ENTITY_DEBUG   (*this) << "This is a message of level MSG_TYPE_DEBUG\n";
    DYNAMIC_GRAPH_ENTITY_INFO    (*this) << "This is a message of level MSG_TYPE_INFO\n";
    DYNAMIC_GRAPH_ENTITY_WARNING (*this) << "This is a message of level MSG_TYPE_WARNING\n";
    DYNAMIC_GRAPH_ENTITY_ERROR   (*this) << "This is a message of level MSG_TYPE_ERROR\n";
    DYNAMIC_GRAPH_ENTITY_DEBUG_STREAM   (*this) << "This is a message of level MSG_TYPE_DEBUG_STREAM\n";
    DYNAMIC_GRAPH_ENTITY_INFO_STREAM    (*this) << "This is a message of level MSG_TYPE_INFO_STREAM\n";
    DYNAMIC_GRAPH_ENTITY_WARNING_STREAM (*this) << "This is a message of level MSG_TYPE_WARNING_STREAM\n";
    DYNAMIC_GRAPH_ENTITY_ERROR_STREAM   (*this) << "This is a message of level MSG_TYPE_ERROR_STREAM\n";
    logger().stream(MSG_TYPE_ERROR) << "end update\n";
    return res;
  }

  void act() {
    m_sigdSIN.accessCopy();
  }
};
DYNAMICGRAPH_FACTORY_ENTITY_PLUGIN(CustomEntity, "CustomEntity");
}  // namespace dynamicgraph
