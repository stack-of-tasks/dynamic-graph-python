#ifndef DYNAMIC_GRAPH_PYTHON_MODULE_HH
#define DYNAMIC_GRAPH_PYTHON_MODULE_HH

#include <boost/python.hpp>
#include <boost/mpl/for_each.hpp>

#include <dynamic-graph/entity.h>
#include <dynamic-graph/python/dynamic-graph-py.hh>

namespace dynamicgraph {
namespace python {

constexpr int AddSignals  = 1;
constexpr int AddCommands = 2;

/// \tparam Options by default, all the signals and commands are added as
///         attribute to the Python object. This behaviour works fine for
///         entities that have static commands and signals.
///         If some commands or signals are added or removed dynamiccally, then
///         it is better to disable the default behaviour and handle it
///         specifically.
template<typename T,
  typename bases = boost::python::bases<dynamicgraph::Entity>,
  int Options = AddCommands | AddSignals>
inline auto exposeEntity ()
{
  //std::string hiddenClassName ("_" + T::CLASS_NAME);
  std::string hiddenClassName (T::CLASS_NAME);
  namespace bp = boost::python;
  bp::class_<T, bases, boost::noncopyable > obj (hiddenClassName.c_str(),
      bp::init<std::string>());
  /* TODO at the moment, I couldn't easily find a way to define a Python constructor
   * that would create the entity via the factory and then populate the
   * python object with its commands.
   * This is achieved with a factory function of the same name.
  obj.def ("__init__", bp::raw_function(+[](bp::object args, bp::dict) {
          if (bp::len(args) != 2)
            throw std::length_error("Expected 2 arguments.");
          bp::object self = args[0];
          self.attr("__init__")(bp::extract<std::string>(args[1]));
          Entity* ent = entity::create(T::CLASS_NAME.c_str(), name);
          if (dynamic_cast<T*>(ent) == NULL)
            std::cout << "foo" << std::endl;
          assert(dynamic_cast<T*>(ent) != NULL);
          self = bp::object(bp::ptr(static_cast<T*>(ent)));
          //dynamicgraph::Entity& unused = bp::extract<dynamicgraph::Entity&>(self);
          //entity::addCommands(self);
        })
  ;
  */
  bp::def(T::CLASS_NAME.c_str(), +[](const char* name) -> bp::object {
          Entity* ent = entity::create(T::CLASS_NAME.c_str(), name);
          assert(dynamic_cast<T*>(ent) != NULL);
          bp::object obj(bp::ptr(static_cast<T*>(ent)));
          if (Options & AddCommands) entity::addCommands(obj);
          if (Options & AddSignals)  entity::addSignals(obj);
          return obj;
        });
  return obj;
}

} // namespace python
} // namespace dynamicgraph

#endif // DYNAMIC_GRAPH_PYTHON_MODULE_HH
