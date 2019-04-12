# Olivier Stasse
# 2019 CNRS
# 
import sys, os
# Put local python module at first priority
sys.path.insert(0,os.getcwd()+'/../src')
sys.path.insert(0,os.getcwd())

print(os.getcwd())
from dynamic_graph_tests.custom_entity import *
from dynamic_graph.entity import VerbosityLevel

aCustomEntity = CustomEntity("a_custom_entity")
print(dir(aCustomEntity))
aCustomEntity.signals()
aCustomEntity.setLoggerVerbosityLevel(VerbosityLevel.VERBOSITY_INFO_WARNING_ERROR)
print(aCustomEntity.getLoggerVerbosityLevel())
aCustomEntity.setLoggerVerbosityLevel(VerbosityLevel.VERBOSITY_WARNING_ERROR)
print(aCustomEntity.getLoggerVerbosityLevel())
aCustomEntity.setLoggerVerbosityLevel(VerbosityLevel.VERBOSITY_ERROR)
print(aCustomEntity.getLoggerVerbosityLevel())
aCustomEntity.setLoggerVerbosityLevel(VerbosityLevel.VERBOSITY_NONE)
print(aCustomEntity.getLoggerVerbosityLevel())
aCustomEntity.setLoggerVerbosityLevel(VerbosityLevel.VERBOSITY_ALL)
print(aCustomEntity.getLoggerVerbosityLevel())

