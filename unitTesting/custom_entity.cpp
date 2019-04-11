/* Copyright 2010-2019 LAAS, CNRS
 * Thomas Moulard.
 *
 */

#define ENABLE_RT_LOG

#include <sstream>
#include <dynamic-graph/entity.h>
#include <dynamic-graph/exception-factory.h>
#include "dynamic-graph/factory.h"
#include "dynamic-graph/pool.h"
#include <dynamic-graph/real-time-logger.h>
#include <dynamic-graph/signal-ptr.h>
#include <dynamic-graph/signal-time-dependent.h>


namespace dynamicgraph
{
  class CustomEntity : public Entity
  {
  public:
    dynamicgraph::SignalPtr<double, int> m_sigdSIN;
    dynamicgraph::SignalTimeDependent<double, int> m_sigdTimeDepSOUT;

    static const std::string CLASS_NAME;
    virtual const std::string& getClassName () const
    {
      return CLASS_NAME;
    }
    CustomEntity (const std::string n)
      : Entity (n)
      ,m_sigdSIN(NULL,"CustomEntity("+name+")::input(double)::in_double")
      ,m_sigdTimeDepSOUT(boost::bind(&CustomEntity::update,this,_1,_2),
			 m_sigdSIN,
			 "CustomEntity("+name+")::input(double)::out_double")

    {
    }

    void addSignal()
    {
      signalRegistration(m_sigdSIN << m_sigdTimeDepSOUT);
    }

    void rmValidSignal()
    {
      signalDeregistration("in_double");
      signalDeregistration("out_double");
    }

    double & update(double &res, const int &inTime)
    {
      const double &aDouble = m_sigdSIN(inTime);
      res = aDouble;
      return res;
    }

  };
  DYNAMICGRAPH_FACTORY_ENTITY_PLUGIN (CustomEntity,"CustomEntity");
}
