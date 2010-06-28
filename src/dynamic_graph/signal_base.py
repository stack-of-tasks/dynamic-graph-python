"""
  Copyright (C) 2010 CNRS

  Author: Florent Lamiraux
"""
import wrap

class SignalBase:
    """
    This class binds dynamicgraph::SignalBase<int> C++ class
    """
    def __init__(self, name, object = None) :
        """
        Constructor: create and store a pointer to a C++ Entity object if
          - the constructor is not called by a child class and
          - object parameter is not provided.
        If object parameter is provided, the class wraps an already existing
        C++ object.
        """
        if object is None :
            if not hasattr(self, "object") :
                self.object = wrap.create_signal_base(name)
        else :
            self.object = object

    @property
    def time(self) :
        """
        Get time of signal
        """
        return wrap.signalBaseGetTime(self.object)

