// Copyright 2019, Olivier Stasse, LAAS-CNRS.
//
// See LICENSE

#include <iostream>

#define ENABLE_RT_LOG
#include <dynamic-graph/real-time-logger.h>

#include <map>
#include <Python.h>
#include <dynamic-graph/pool.h>
#include <dynamic-graph/entity.h>
#include <vector>
#include "dynamic-graph/python/exception.hh"

#include <boost/shared_ptr.hpp>
#include "dynamic-graph/python/dynamic-graph-py.hh"

typedef boost::shared_ptr<std::ofstream> ofstreamShrPtr;

namespace dynamicgraph {
namespace python {

namespace debug {

std::map<std::string, ofstreamShrPtr> mapOfFiles_;

PyObject* addLoggerFileOutputStream(PyObject* /*self*/, PyObject* args) {
  char* filename;
  if (!PyArg_ParseTuple(args, "s", &filename)) return NULL;
  std::string sfilename(filename);
  try {
    std::ofstream* aofs = new std::ofstream;
    ofstreamShrPtr ofs_shrptr = boost::shared_ptr<std::ofstream>(aofs);
    aofs->open(filename, std::ofstream::out);
    dynamicgraph::RealTimeLogger::instance();
    dgADD_OSTREAM_TO_RTLOG(*aofs);
    dgRTLOG() << "Added " << filename << " as an output stream \n";
    mapOfFiles_[sfilename] = ofs_shrptr;
  }
  CATCH_ALL_EXCEPTIONS();
  return Py_BuildValue("");
}

PyObject* closeLoggerFileOutputStream(PyObject* /*self*/, PyObject* /*args */) {
  try {
    for (std::map<std::string, ofstreamShrPtr>::iterator it = mapOfFiles_.begin(); it != mapOfFiles_.end(); ++it) {
      it->second->close();
    }
  }
  CATCH_ALL_EXCEPTIONS();
  return Py_BuildValue("");
}

PyObject* addLoggerCoutOutputStream(PyObject* /*self*/, PyObject* /*args*/) {
  try {
    dgADD_OSTREAM_TO_RTLOG(std::cout);
  }
  CATCH_ALL_EXCEPTIONS();
  return Py_BuildValue("");
}

PyObject* realTimeLoggerDestroy(PyObject* /*self*/, PyObject* /*args*/) {
  try {
    RealTimeLogger::destroy();
  }
  CATCH_ALL_EXCEPTIONS();
  return Py_BuildValue("");
}

PyObject* realTimeLoggerSpinOnce(PyObject* /*self*/, PyObject* /*args*/) {
  try {
    RealTimeLogger::instance().spinOnce();
  }
  CATCH_ALL_EXCEPTIONS();
  return Py_BuildValue("");
}

PyObject* realTimeLoggerInstance(PyObject* /*self*/, PyObject* /*args*/) {
  try {
    RealTimeLogger::instance();
  }
  CATCH_ALL_EXCEPTIONS();
  return Py_BuildValue("");
}

}  // namespace debug
}  // namespace python
}  // namespace dynamicgraph
