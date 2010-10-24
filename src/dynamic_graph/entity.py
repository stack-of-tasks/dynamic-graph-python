"""
  Copyright (C) 2010 CNRS

  Author: Florent Lamiraux
"""
import wrap, signal_base

entityClassNameList = []

def initEntity(self, name):
    """
    Common constructor of Entity classes
    """
    Entity.__init__(self, self.class_name, name)

def updateEntityClasses(dictionary):
    cxx_entityList = wrap.factory_get_entity_class_list()
    for e in filter(lambda x: not x in entityClassNameList, cxx_entityList):
        class metacls(type):
            def __new__(mcs, name, bases, dict):
                return type.__new__(mcs, name, bases, dict)

        # Create new class
        a = metacls(e, (Entity,), {})
        # Store class name in class member
        a.class_name = e
        # set class constructor
        setattr(a, '__init__', initEntity)
        # Store new class in dictionary with class name
        dictionary[e] = a
        # Store class name in local list
        entityClassNameList.append(e)


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

