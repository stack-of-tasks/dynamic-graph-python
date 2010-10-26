"""
  Copyright (C) 2010 CNRS

  Author: Florent Lamiraux
"""
import wrap
import re

def stringToTuple(vector):
    """
    Transform a string of format '[n](x_1, x_2, ..., x_{n-1} into a list of
    numbers
    """
    # Find vector length
    a = re.search('[\d]', vector)
    size = int(a.group(0))
    format = '\('
    for i in range(size):
        format += '(.*)'
        if i != size-1:
            format += ','
    format += '\)'
    a = re.search(format, vector)
    res = []
    for i in range(1, size+1):
        res.append(float(a.group(i)))
    return tuple(res)


class SignalBase (object) :
    """
    This class binds dynamicgraph::SignalBase<int> C++ class
    """

    object = None

    def __init__(self, name, object = None) :
        """
        Constructor: if not called by a child class, create and store a pointer
        to a C++ SignalBase<int> object.
        """
        if object :
            self.object = object
        if not self.object :
            self.object = wrap.create_signal_base(self, name)

    @property
    def time(self) :
        """
        Get time of signal
        """
        return wrap.signalBaseGetTime(self.object)

    @property
    def value(self) :
        """
        Read the value of a signal
        """
        return wrap.signal_base_get_value(self.object)

    @value.setter
    def value(self, val) :
        """
        Set the signal as a constant signal with given value.
        If the signal is plugged, it will be unplugged
        """
        return wrap.signal_base_set_value(self.object, val)
