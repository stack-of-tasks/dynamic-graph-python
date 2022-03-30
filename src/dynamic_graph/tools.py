# -*- coding: utf-8 -*-
# Copyright 2011, Florent Lamiraux, Thomas Moulard, JRL, CNRS/AIST

from __future__ import print_function


def addTrace(robot, trace, entityName, signalName, autoRecompute=True):
    """
    Add a signal to a tracer and recompute it automatically if necessary.
    """
    signal = "{0}.{1}".format(entityName, signalName)
    filename = "{0}-{1}".format(entityName, signalName).replace("/", "_")
    trace.add(signal, filename)
    if autoRecompute:
        robot.device.after.addSignal(signal)
