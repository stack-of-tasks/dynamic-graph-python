import sys, os
# Put local python module at first priority
sys.path.insert(0,os.getcwd()+'/../src')
sys.path.insert(0,os.getcwd())

print(os.getcwd())
from dynamic_graph_tests.custom_entity import *
from dynamic_graph.entity import VerbosityLevel

aCustomEntity = CustomEntity("a_custom_entity")

aCustomEntity.setLoggerVerbosityLevel(VerbosityLevel.VERBOSITY_WARNING_ERROR)



print(aCustomEntity.getLoggerVerbosityLevel())


