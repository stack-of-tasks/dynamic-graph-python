// Copyright 2019, Olivier Stasse, LAAS-CNRS.
//
// See LICENSE

#include <iostream>

#define ENABLE_RT_LOG
#include <dynamic-graph/entity.h>
#include <dynamic-graph/pool.h>
#include <dynamic-graph/real-time-logger.h>

#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>

#include "dynamic-graph/python/dynamic-graph-py.hh"

typedef boost::shared_ptr<std::ofstream> ofstreamShrPtr;

namespace dynamicgraph {
namespace python {

namespace debug {

std::map<std::string, ofstreamShrPtr> mapOfFiles_;

void addLoggerFileOutputStream(const char* filename) {
  std::ofstream* aofs = new std::ofstream;
  ofstreamShrPtr ofs_shrptr = boost::shared_ptr<std::ofstream>(aofs);
  aofs->open(filename, std::ofstream::out);
  dynamicgraph::RealTimeLogger::instance();
  dgADD_OSTREAM_TO_RTLOG(*aofs);
  dgRTLOG() << "Added " << filename << " as an output stream \n";
  mapOfFiles_[filename] = ofs_shrptr;
}

void closeLoggerFileOutputStream() {
  for (const auto& el : mapOfFiles_) el.second->close();
}

void addLoggerCoutOutputStream() { dgADD_OSTREAM_TO_RTLOG(std::cout); }

void realTimeLoggerDestroy() { RealTimeLogger::destroy(); }

void realTimeLoggerSpinOnce() { RealTimeLogger::instance().spinOnce(); }

void realTimeLoggerInstance() { RealTimeLogger::instance(); }

}  // namespace debug
}  // namespace python
}  // namespace dynamicgraph
