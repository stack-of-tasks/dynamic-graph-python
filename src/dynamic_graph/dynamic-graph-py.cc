// Copyright 2010, Florent Lamiraux, Thomas Moulard, LAAS-CNRS.

#include "dynamic-graph/python/dynamic-graph-py.hh"

#include <dynamic-graph/command.h>
#include <dynamic-graph/debug.h>
#include <dynamic-graph/entity.h>
#include <dynamic-graph/exception-factory.h>
#include <dynamic-graph/factory.h>
#include <dynamic-graph/pool.h>
#include <dynamic-graph/signal-base.h>
#include <dynamic-graph/signal-time-dependent.h>
#include <dynamic-graph/signal.h>
#include <dynamic-graph/tracer.h>

#include <Eigen/Geometry>
#include <boost/python.hpp>
#include <boost/python/raw_function.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <eigenpy/eigenpy.hpp>
#include <eigenpy/geometry.hpp>
#include <iostream>
#include <sstream>

#include "dynamic-graph/python/convert-dg-to-py.hh"
#include "dynamic-graph/python/module.hh"
#include "dynamic-graph/python/signal-wrapper.hh"

namespace dynamicgraph {
namespace python {

/**
   \brief plug a signal into another one.
*/
void plug(SignalBase<sigtime_t>* signalOut, SignalBase<sigtime_t>* signalIn) {
  signalIn->plug(signalOut);
}

void enableTrace(bool enable, const char* filename) {
  if (enable)
    DebugTrace::openFile(filename);
  else
    DebugTrace::closeFile(filename);
}

}  // namespace python
}  // namespace dynamicgraph

namespace bp = boost::python;
namespace dg = dynamicgraph;

typedef bp::return_value_policy<bp::manage_new_object> manage_new_object;
typedef bp::return_value_policy<bp::reference_existing_object>
    reference_existing_object;

typedef dg::PoolStorage::Entities MapOfEntities;

struct MapOfEntitiesPairToPythonConverter {
  static PyObject* convert(const MapOfEntities::value_type& pair) {
    return bp::incref(bp::make_tuple(pair.first, bp::ptr(pair.second)).ptr());
  }
};

MapOfEntities* getEntityMap() {
  return const_cast<MapOfEntities*>(
      &dg::PoolStorage::getInstance()->getEntityMap());
}

dg::SignalBase<dg::sigtime_t>* getSignal(dg::Entity& e, const std::string& name)
{
  return &e.getSignal(name);
}

class PythonEntity : public dg::Entity {
  DYNAMIC_GRAPH_ENTITY_DECL();

 public:
  using dg::Entity::Entity;

  void signalRegistration(dg::SignalBase<dg::sigtime_t>& signal) {
    dg::Entity::signalRegistration(signal);
  }
  void signalDeregistration(const std::string& name) {
    dg::Entity::signalDeregistration(name);
  }
};

DYNAMICGRAPH_FACTORY_ENTITY_PLUGIN(PythonEntity, "PythonEntity");

void exposeEntityBase() {
  using namespace dynamicgraph;
  bp::enum_<LoggerVerbosity>("LoggerVerbosity")
      .value("VERBOSITY_ALL", VERBOSITY_ALL)
      .value("VERBOSITY_INFO_WARNING_ERROR", VERBOSITY_INFO_WARNING_ERROR)
      .value("VERBOSITY_WARNING_ERROR", VERBOSITY_WARNING_ERROR)
      .value("VERBOSITY_ERROR", VERBOSITY_ERROR)
      .value("VERBOSITY_NONE", VERBOSITY_NONE)
      .export_values();

  bp::class_<Entity, boost::noncopyable>("Entity", bp::no_init)
      .add_property("name",
                    bp::make_function(
                        &Entity::getName,
                        bp::return_value_policy<bp::copy_const_reference>()))
      .add_property("className",
                    bp::make_function(
                        &Entity::getClassName,
                        bp::return_value_policy<bp::copy_const_reference>()),
                    "the class name of the Entity")
      .add_property("__doc__", &Entity::getDocString)

      .def("setLoggerVerbosityLevel", &Entity::setLoggerVerbosityLevel)
      .def("getLoggerVerbosityLevel", &Entity::getLoggerVerbosityLevel)
      .add_property("loggerVerbosityLevel", &Entity::setLoggerVerbosityLevel,
                    &Entity::getLoggerVerbosityLevel,
                    "the verbosity level of the entity")
      .def("setTimeSample", &Entity::setTimeSample)
      .def("getTimeSample", &Entity::getTimeSample)
      .add_property("timeSample", &Entity::getTimeSample,
                    &Entity::setTimeSample,
                    "the time sample for printing debugging information")
      .def("setStreamPrintPeriod", &Entity::setStreamPrintPeriod)
      .def("getStreamPrintPeriod", &Entity::getStreamPrintPeriod)
      .add_property("streamPrintPeriod", &Entity::getStreamPrintPeriod,
                    &Entity::setStreamPrintPeriod,
                    "set the period at which debugging information are printed")

      .def(
          "__str__",
          +[](const Entity& e) -> std::string {
            std::ostringstream os;
            e.display(os);
            return os.str();
          })
      .def(
          "signals",
          +[](const Entity& e) -> bp::list {
            bp::list ret;
            for (auto& el : e.getSignalMap()) ret.append(bp::ptr(el.second));
            return ret;
          },
          "Return the list of signals.")
      //.def("signal", +[](Entity& e, const std::string &name) { return
      //&e.getSignal(name); },
      // reference_existing_object())
      .def("signal", &getSignal, reference_existing_object(),
           "get signal by name from an Entity", bp::arg("name"))
      .def("hasSignal", &Entity::hasSignal,
           "return True if the entity has a signal with the given name")

      .def(
          "displaySignals",
          +[](const Entity& e) {
            Entity::SignalMap signals(e.getSignalMap());
            std::cout << "--- <" << e.getName();
            if (signals.empty())
              std::cout << "> has no signal\n";
            else
              std::cout << "> signal list:\n";
            for (const auto& el : signals)
              el.second->display(std::cout << "    |-- <") << '\n';
          },
          "Print the list of signals into standard output: temporary.")

      /*
      .def("__getattr__", +[](Entity& e, const std::string &name) ->
      SignalBase<int>* { return &e.getSignal(name); },
          reference_existing_object())
      def __getattr__(self, name):
          try:
              return self.signal(name)
          except Exception:
              try:
                  object.__getattr__(self, name)
              except AttributeError:
                  raise AttributeError("'%s' entity has no attribute %s\n" %
      (self.name, name) + '  entity attributes are usually either\n' + '    -
      commands,\n' + '    - signals or,\n' + '    - user defined attributes')
                                       */
      /*
      .def("__setattr__", +[](bp::object self, const std::string &name,
      bp::object value) { Entity& e = bp::extract<Entity&> (self); if
      (e.hasSignal(name)) throw std::invalid_argument(name + " already
      designates a signal. " "It is not advised to set a new attribute of the
      same name.");
            // TODO How do you do that ? I am sure it is possible.
            //object.__setattr__(self, name, value)
          })
          */

      /* TODO ?
      def boundNewCommand(self, cmdName):
          """
          At construction, all existing commands are bound directly in the
      class. This method enables to bound new commands dynamically. These new
      bounds are not made with the class, but directly with the object instance.
          """
      def boundAllNewCommands(self):
          """
          For all commands that are not attribute of the object instance nor of
      the class, a new attribute of the instance is created to bound the
      command.
          """
          */

      // For backward compat
      .add_static_property(
          "entities",
          bp::make_function(&getEntityMap, reference_existing_object()));

  python::exposeEntity<PythonEntity, bp::bases<Entity>, 0>()
      .def("signalRegistration", &PythonEntity::signalRegistration)
      .def("signalDeregistration", &PythonEntity::signalDeregistration);

  python::exposeEntity<python::PythonSignalContainer, bp::bases<Entity>, 0>()
      .def("rmSignal", &python::PythonSignalContainer::rmSignal,
           "Remove a signal", bp::arg("signal_name"));
}

void exposeCommand() {
  using dg::command::Command;
  bp::class_<Command, boost::noncopyable>("Command", bp::no_init)
      .def("__call__", bp::raw_function(dg::python::entity::executeCmd, 1),
           "execute the command")
      .add_property("__doc__", &Command::getDocstring);
}

void exposeOldAPI() {
  bp::def("plug", dynamicgraph::python::plug,
          "plug an output signal into an input signal",
          (bp::arg("signalOut"), "signalIn"));
  bp::def("enableTrace", dynamicgraph::python::enableTrace,
          "Enable or disable tracing debug info in a file");
  // Signals
  bp::def("create_signal_wrapper",
          dynamicgraph::python::signalBase::createSignalWrapper,
          reference_existing_object(), "create a SignalWrapper C++ object");
  // Entity
  bp::def("factory_get_entity_class_list",
          dynamicgraph::python::factory::getEntityClassList,
          "return the list of entity classes");
  bp::def("writeGraph", dynamicgraph::python::pool::writeGraph,
          "Write the graph of entities in a filename.");
  bp::def("get_entity_list", dynamicgraph::python::pool::getEntityList,
          "return the list of instanciated entities");
  bp::def("addLoggerFileOutputStream",
          dynamicgraph::python::debug::addLoggerFileOutputStream,
          "add a output file stream to the logger by filename");
  bp::def("addLoggerCoutOutputStream",
          dynamicgraph::python::debug::addLoggerCoutOutputStream,
          "add std::cout as output stream to the logger");
  bp::def("closeLoggerFileOutputStream",
          dynamicgraph::python::debug::closeLoggerFileOutputStream,
          "close all the loggers file output streams.");
  bp::def("real_time_logger_destroy",
          dynamicgraph::python::debug::realTimeLoggerDestroy,
          "Destroy the real time logger.");
  bp::def("real_time_logger_spin_once",
          dynamicgraph::python::debug::realTimeLoggerSpinOnce,
          "Destroy the real time logger.");
  bp::def("real_time_logger_instance",
          dynamicgraph::python::debug::realTimeLoggerInstance,
          "Starts the real time logger.");
}

void enableEigenPy() {
  eigenpy::enableEigenPy();

  if (!eigenpy::register_symbolic_link_to_registered_type<Eigen::Quaterniond>())
    eigenpy::exposeQuaternion();
  if (!eigenpy::register_symbolic_link_to_registered_type<Eigen::AngleAxisd>())
    eigenpy::exposeAngleAxis();

  eigenpy::enableEigenPySpecific<Eigen::Matrix4d>();
}

BOOST_PYTHON_MODULE(wrap) {
  enableEigenPy();

  exposeOldAPI();

  dg::python::exposeSignals();
  exposeEntityBase();
  exposeCommand();

  typedef dg::PoolStorage::Entities MapOfEntities;
  bp::class_<MapOfEntities>("MapOfEntities")
      .def("__len__", &MapOfEntities::size)
      .def(
          "keys",
          +[](const MapOfEntities& m) -> bp::tuple {
            bp::list res;
            for (const auto& el : m) res.append(el.first);
            return bp::tuple(res);
          })
      .def(
          "values",
          +[](const MapOfEntities& m) -> bp::tuple {
            bp::list res;
            for (const auto& el : m) res.append(bp::ptr(el.second));
            return bp::tuple(res);
          })
      .def("__getitem__",
           static_cast<dg::Entity*& (MapOfEntities::*)(const std::string& k)>(
               &MapOfEntities::at),
           reference_existing_object())
      .def(
          "__setitem__", +[](MapOfEntities& m, const std::string& n,
                             dg::Entity* e) { m.emplace(n, e); })
      .def("__iter__", bp::iterator<MapOfEntities>())
      .def(
          "__contains__",
          +[](const MapOfEntities& m, const std::string& n) -> bool {
            return m.count(n);
          });
  bp::to_python_converter<MapOfEntities::value_type,
                          MapOfEntitiesPairToPythonConverter>();
}
