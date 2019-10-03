# Olivier Stasse
# 2019 CNRS
#
import os
import time

from dynamic_graph import (addLoggerCoutOutputStream, addLoggerFileOutputStream, closeLoggerFileOutputStream,
                           real_time_logger_destroy, real_time_logger_spin_once)
from dynamic_graph.entity import VerbosityLevel
from dynamic_graph_tests.custom_entity import CustomEntity

print(os.getcwd())

# Starts the real time logger instance

aCustomEntity = CustomEntity("a_custom_entity")

addLoggerFileOutputStream("/tmp/output.dat")
aCustomEntity.signals()

aCustomEntity.setTimeSample(0.001)
print(aCustomEntity.getTimeSample())
aCustomEntity.setStreamPrintPeriod(0.002)
print(aCustomEntity.getStreamPrintPeriod())

aCustomEntity.setLoggerVerbosityLevel(VerbosityLevel.VERBOSITY_INFO_WARNING_ERROR)
print(aCustomEntity.getLoggerVerbosityLevel())
for i in range(0, 5):
    aCustomEntity.in_double.value = i
    aCustomEntity.out_double.recompute(i)
    real_time_logger_spin_once()
    print(i)
time.sleep(1)

aCustomEntity.setLoggerVerbosityLevel(VerbosityLevel.VERBOSITY_WARNING_ERROR)
print(aCustomEntity.getLoggerVerbosityLevel())
for i in range(5, 10):
    aCustomEntity.in_double.value = i
    aCustomEntity.out_double.recompute(i)
    real_time_logger_spin_once()
time.sleep(1)

aCustomEntity.setLoggerVerbosityLevel(VerbosityLevel.VERBOSITY_ERROR)
print(aCustomEntity.getLoggerVerbosityLevel())
for i in range(10, 15):
    aCustomEntity.in_double.value = i
    aCustomEntity.out_double.recompute(i)
    real_time_logger_spin_once()
time.sleep(1)
addLoggerCoutOutputStream()
time.sleep(1)
aCustomEntity.setLoggerVerbosityLevel(VerbosityLevel.VERBOSITY_NONE)
print(aCustomEntity.getLoggerVerbosityLevel())
for i in range(15, 20):
    aCustomEntity.in_double.value = i
    aCustomEntity.out_double.recompute(i)
    real_time_logger_spin_once()
time.sleep(1)

aCustomEntity.setLoggerVerbosityLevel(VerbosityLevel.VERBOSITY_ALL)
print(aCustomEntity.getLoggerVerbosityLevel())
for i in range(20, 25):
    aCustomEntity.in_double.value = i
    aCustomEntity.out_double.recompute(i)
    real_time_logger_spin_once()

# End the real time logger
real_time_logger_destroy()

# Close all the output stream
closeLoggerFileOutputStream()
