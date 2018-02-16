// Copyright (c) 2018, Joseph Mirabel
// Authors: Joseph Mirabel (joseph.mirabel@laas.fr)
//
// This file is part of dynamic-graph-python.
// dynamic-graph-python is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// dynamic-graph-python is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Lesser Public License for more details.  You should have
// received a copy of the GNU Lesser General Public License along with
// dynamic-graph-python. If not, see <http://www.gnu.org/licenses/>.

#include <Python.h>

#include <dynamic-graph/linear-algebra.h>
#include <dynamic-graph/signal.h>
#include <dynamic-graph/entity.h>

namespace dynamicgraph {
  namespace python {
    namespace signalWrapper {
      void convert (PyObject* o, int   & v);
      void convert (PyObject* o, bool  & v);
      void convert (PyObject* o, float & v);
      void convert (PyObject* o, double& v);
      // void convert (PyObject* o, std::string& v);
      void convert (PyObject* o, Vector     & v);
      // void convert (PyObject* o, Eigen::MatrixXd& v);
      // void convert (PyObject* o, Eigen::Matrix4d& v);
    }

    class PythonSignalContainer : public Entity
    {
      DYNAMIC_GRAPH_ENTITY_DECL();

      public:
        PythonSignalContainer (const std::string& name) : Entity (name) {};

        void signalRegistration (const SignalArray<int>& signals)
        {
          Entity::signalRegistration (signals);
        }
    };

    template <class T, class Time>
    class SignalWrapper : public Signal<T, Time>
    {
      public:
        typedef Signal<T,Time> parent_t;

        static bool checkCallable (PyObject* c, std::string& error);

        SignalWrapper (std::string name, PyObject* _callable) : 
          parent_t (name)
          , callable (_callable)
        {
          typedef boost::function2<T&,T&,Time> function_t;
          Py_INCREF(callable);
          function_t f = boost::bind (&SignalWrapper::call, this, _1, _2);
          this->setFunction (f);
        }

        virtual ~SignalWrapper ()
        {
          Py_DECREF(callable);
        };

      private:
        T& call (T& value, Time t)
        {
          char format[] = "i";
          PyObject* obj = PyObject_CallFunction(callable, format, t);
          if (obj == NULL)
            std::cerr << "Could not call callable" << std::endl;
          else {
            signalWrapper::convert (obj, value);
            Py_DECREF(obj);
          }
          return value;
        }
        PyObject* callable;
    };

  } // namespace dynamicgraph
} // namespace python
