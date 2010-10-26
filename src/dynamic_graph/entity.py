"""
  Copyright (C) 2010 CNRS

  Author: Florent Lamiraux
"""
import wrap, signal_base

entityClassNameList = []

def commandMethod(name) :
    def method(self, *arg):
        return wrap.entity_execute_command(self.object, name, arg)
    return method

def initEntity(self, name):
    """
    Common constructor of Entity classes
    """
    Entity.__init__(self, self.className, name)
    if not self.__class__.commandCreated:
        # Get list of commands of the Entity object
        commands = wrap.entity_list_commands(self.object)
        # for each command, add a method with the name of the command
        for command in commands:
            print ('adding method %s in class %s' %(command, self.__class__))
            setattr(self.__class__, command, commandMethod(command))
        self.__class__.commandCreated = True

def updateEntityClasses(dictionary):
    cxx_entityList = wrap.factory_get_entity_class_list()
    for e in filter(lambda x: not x in entityClassNameList, cxx_entityList):
        class metacls(type):
            def __new__(mcs, name, bases, dict):
                return type.__new__(mcs, name, bases, dict)

        # Create new class
        a = metacls(e, (Entity,), {})
        # Store class name in class member
        a.className = e
        # set class constructor
        setattr(a, '__init__', initEntity)
        # set class attribute to store whether command methods have been created
        setattr(a, 'commandCreated', False)
        #

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

