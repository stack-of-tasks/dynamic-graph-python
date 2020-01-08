#include "dynamic-graph/python/interpreter.hh"

int main(int argc, char* argv[]) {
  dynamicgraph::python::Interpreter interp;
  return interp.runMain(argc, argv);
}
