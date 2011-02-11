// -*- mode: c++ -*-
// Copyright 2011, Florent Lamiraux, CNRS.
//
// This file is part of dynamic-graph-python.
// dynamic-graph is free software: you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// dynamic-graph is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Lesser Public License for more details.  You should have
// received a copy of the GNU Lesser General Public License along with
// dynamic-graph. If not, see <http://www.gnu.org/licenses/>.

#undef _POSIX_C_SOURCE
#undef _XOPEN_SOURCE
#include <Python.h>
#include <string>
#include "dynamic-graph/python/api.hh"

#ifndef DYNAMIC_GRAPH_PYTHON_INTERPRETER_H
# define DYNAMIC_GRAPH_PYTHON_INTERPRETER_H

#include "dynamic-graph/python/api.hh"

namespace dynamicgraph {
  namespace python {
      ///
      /// This class implements a basis python interpreter.
      ///
      /// String sent to method python are interpreted by an onboard python
      /// interpreter.
    class DYNAMIC_GRAPH_PYTHON_DLLAPI Interpreter
    {
    public:
      Interpreter();
      ~Interpreter();
      /// \brief Method to start python interperter.
      /// \param command string to execute
      std::string python( const std::string& command );

      /// \brief Method to exectue a python script.
      /// \param filename the filename
      void runPythonFile( std::string filename );
      void runMain( void );

      /// \brief Process input stream to send relevant blocks to python
      /// \param stream input stream
      std::string processStream(std::istream& stream, std::ostream& os);

    private:
      /// Pointer to the dictionary of global variables
      PyObject* globals_;
      /// Pointer to the dictionary of local variables
      PyObject* locals_;
      PyObject* mainmod_;
      PyObject* traceback_format_exception_;
    };
  } // namespace python
} // namespace dynamicgraph
#endif // DYNAMIC_GRAPH_PYTHON_INTERPRETER_H
