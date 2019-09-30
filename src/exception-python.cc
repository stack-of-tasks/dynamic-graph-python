/*
 * Copyright 2010,
 * François Bleibel,
 * Olivier Stasse,
 *
 * CNRS/AIST
 *
 */

#include <dynamic-graph/python/exception-python.hh>
#include <dynamic-graph/debug.h>
#include <stdarg.h>
#include <cstdio>

namespace dynamicgraph {
namespace python {

/* --------------------------------------------------------------------- */
/* --- CLASS ----------------------------------------------------------- */
/* --------------------------------------------------------------------- */

const std::string ExceptionPython::EXCEPTION_NAME = "Python";

ExceptionPython::ExceptionPython(const ExceptionPython::ErrorCodeEnum& errcode, const std::string& msg)
    : ExceptionAbstract(errcode, msg) {
  dgDEBUGF(15, "Created with message <%s>.", msg.c_str());
  dgDEBUG(1) << "Created with message <%s>." << msg << std::endl;
}

ExceptionPython::ExceptionPython(const ExceptionPython::ErrorCodeEnum& errcode, const std::string& msg,
                                 const char* format, ...)
    : ExceptionAbstract(errcode, msg) {
  va_list args;
  va_start(args, format);

  const unsigned int SIZE = 256;
  char buffer[SIZE];
  vsnprintf(buffer, SIZE, format, args);

  dgDEBUG(15) << "Created "
              << " with message <" << msg << "> and buffer <" << buffer << ">. " << std::endl;

  message += buffer;

  va_end(args);

  dgDEBUG(1) << "Throw exception " << EXCEPTION_NAME << "[#" << errcode << "]: "
             << "<" << message << ">." << std::endl;
}

}  // namespace python
}  // namespace dynamicgraph

/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
