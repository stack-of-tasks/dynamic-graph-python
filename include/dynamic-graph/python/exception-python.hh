// -*- mode: c++ -*-
// Copyright 2010, François Bleibel, Thomas Moulard, Olivier Stasse,
// JRL, CNRS/AIST.

#ifndef DYNAMIC_GRAPH_PYTHON_EXCEPTION_PYTHON_H
#define DYNAMIC_GRAPH_PYTHON_EXCEPTION_PYTHON_H
#include <string>

#include <dynamic-graph/fwd.hh>
#include <dynamic-graph/exception-abstract.h>

// Depending on whether one is building or using the
// library define DLLAPI to import or export.
#if defined(WIN32)
#if defined(wrap_EXPORTS)
#define WRAP_DLLAPI __declspec(dllexport)
#else
#define WRAP_DLLAPI __declspec(dllimport)
#endif
#else
#define WRAP_DLLAPI
#endif

namespace dynamicgraph {
namespace python {

/// \ingroup error
///
/// \brief Generic error class.
class WRAP_DLLAPI ExceptionPython : public ExceptionAbstract {
 public:
  enum ErrorCodeEnum { GENERIC, VALUE_PARSING, VECTOR_PARSING, MATRIX_PARSING, CLASS_INCONSISTENT };

  static const std::string EXCEPTION_NAME;

  explicit ExceptionPython(const ExceptionPython::ErrorCodeEnum& errcode, const std::string& msg = "");

  ExceptionPython(const ExceptionPython::ErrorCodeEnum& errcode, const std::string& msg, const char* format, ...);

  virtual ~ExceptionPython() throw() {}

  virtual const std::string& getExceptionName() const { return ExceptionPython::EXCEPTION_NAME; }
};
}  // end of namespace python
}  // end of namespace dynamicgraph

#endif  //! DYNAMIC_GRAPH_PYTHON_EXCEPTION_PYTHON_H
