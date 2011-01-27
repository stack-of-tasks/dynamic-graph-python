// -*- mode: c++ -*-
// Copyright 2010, François Bleibel, Thomas Moulard, Olivier Stasse,
// JRL, CNRS/AIST.
//
// This file is part of dynamic-graph.
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

#ifndef DYNAMIC_GRAPH_PYTHON_EXCEPTION_PYTHON_H
# define DYNAMIC_GRAPH_PYTHON_EXCEPTION_PYTHON_H
# include <string>

# include <dynamic-graph/fwd.hh>
# include <dynamic-graph/dynamic-graph-api.h>
# include <dynamic-graph/exception-abstract.h>

namespace dynamicgraph {
  namespace python {

    /// \ingroup error
    ///
    /// \brief Generic error class.
    class DYNAMIC_GRAPH_DLLAPI ExceptionPython : public ExceptionAbstract
    {
    public:
      enum ErrorCodeEnum
      {
	GENERIC
	,VALUE_PARSING
	,VECTOR_PARSING
	,MATRIX_PARSING
      };

      static const std::string EXCEPTION_NAME;

      explicit ExceptionPython (const ExceptionPython::ErrorCodeEnum& errcode,
				const std::string & msg = "");

      ExceptionPython (const ExceptionPython::ErrorCodeEnum& errcode,
		       const std::string& msg, const char* format, ...);

      virtual ~ExceptionPython () throw ()
      {}

      virtual const std::string& getExceptionName () const
      {
	return ExceptionPython::EXCEPTION_NAME;
      }
    };
  } // end of namespace python
} // end of namespace dynamicgraph

#endif //! DYNAMIC_GRAPH_PYTHON_EXCEPTION_PYTHON_H
