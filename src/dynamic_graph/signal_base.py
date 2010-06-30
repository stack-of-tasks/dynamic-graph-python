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

