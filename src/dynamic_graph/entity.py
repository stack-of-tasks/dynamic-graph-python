"""
  Copyright (C) 2010 CNRS

  Author: Florent Lamiraux
"""
import wrap, signal_base

class Entity (object) :
    """
    This class binds dynamicgraph::Entity C++ class
    """
    
    object = None

    def __init__(self, className, instanceName):
        """
        Constructor: if not called by a child class, create and store a pointer
        to a C++ Entity object.
        """
        self.object = wrap.create_entity(className, instanceName)

    @property
    def name(self) :
        return wrap.entity_get_name(self.object)

    def signal (self, name) :
        """
        Get a signal of the entity from signal name
        """
        signalPt = wrap.entity_get_signal(self.object, name)
        return signal_base.SignalBase("", signalPt)
        
    def display_signals(self) :
        """
        Write the list of signals into standard output: temporary.
        """
        wrap.entity_display_signals(self.object)

