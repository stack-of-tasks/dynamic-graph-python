"""
  Copyright (C) 2010 CNRS

  Author: Florent Lamiraux
"""
import wrap

class SignalBase:
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
    def value(self) :
        """
        Set the signal as a constant signal with given value.
        If the signal is plugged, it will be unplugged
        """
        return wrap.signal_base_set_value(self.object, value)
