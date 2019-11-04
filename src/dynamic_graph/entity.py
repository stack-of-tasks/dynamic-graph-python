"""
  Copyright (C) 2010 CNRS

  Author: Florent Lamiraux, Nicolas Mansard
"""
from __future__ import print_function

import types
from enum import Enum

from . import signal_base, wrap
from .attrpath import setattrpath

if 'display' not in globals().keys():

    def display(s):
        print(s)


# --- FACTORY ------------------------------------------------------------------


class PyEntityFactoryClass(type):
    """
    The class build dynamically a new class type, and return the reference
    on the class-type object. The class type is not added to any context.
    """
    def __new__(factory, className, bases=(), dict={}):
        if len(bases) == 0:
            # Initialize a basic Entity class
            EntityClass = type.__new__(factory, className, (Entity, ), dict)
            EntityClass.className = className
            EntityClass.__init__ = Entity.initEntity
        else:
            # Initialize a heritated class
            EntityClass = type.__new__(factory, className, bases, dict)
            for c in bases:
                if issubclass(c, Entity):
                    EntityClass.className = c.className
                    break
        EntityClass.commandCreated = False
        return EntityClass


def PyEntityFactory(className, context):
    """
    Build a new class type by calling the factory, and add it
    to the given context.
    """
    EntityClass = PyEntityFactoryClass(className)
    context[className] = EntityClass
    return EntityClass


def updateEntityClasses(dictionary):
    """
    For all c++entity types that are not in the pyentity class list
    (entityClassNameList) run the factory and store the new type in the given
    context (dictionary).
    """
    cxx_entityList = wrap.factory_get_entity_class_list()
    for e in filter(lambda x: x not in Entity.entityClassNameList, cxx_entityList):
        # Store new class in dictionary with class name
        PyEntityFactory(e, dictionary)
        # Store class name in local list
        Entity.entityClassNameList.append(e)


# --- ENTITY -------------------------------------------------------------------


class VerbosityLevel(Enum):
    """
    Enum class for setVerbosityLevel
    """
    VERBOSITY_ALL = 0
    VERBOSITY_INFO_WARNING_ERROR = 1
    VERBOSITY_WARNING_ERROR = 2
    VERBOSITY_ERROR = 3
    VERBOSITY_NONE = 4


class Entity(object):
    """
    This class binds dynamicgraph::Entity C++ class
    """

    obj = None
    """
    Store list of entities created via python
    """
    entities = dict()

    def __init__(self, className, instanceName):
        """
        Constructor: if not called by a child class, create and store a pointer
        to a C++ Entity object.
        """
        object.__setattr__(self, 'obj', wrap.create_entity(className, instanceName))
        Entity.entities[instanceName] = self

    @staticmethod
    def initEntity(self, name):
        """
        Common constructor of specialized Entity classes. This function is bound
        by the factory to each new class derivated from the Entity class as the
        constructor of the new class.
        """
        Entity.__init__(self, self.className, name)
        if not self.__class__.commandCreated:
            self.boundClassCommands()
            self.__class__.__doc__ = wrap.entity_get_docstring(self.obj)
            self.__class__.commandCreated = True

    @property
    def name(self):
        return wrap.entity_get_name(self.obj)

    @property
    def className(self):
        return wrap.entity_get_class_name(self.obj)

    def __str__(self):
        return wrap.display_entity(self.obj)

    def signal(self, name):
        """
        Get a signal of the entity from signal name
        """
        signalPt = wrap.entity_get_signal(self.obj, name)
        return signal_base.SignalBase(name="", obj=signalPt)

    def hasSignal(self, name):
        """
        Indicates if a signal with the given name exists in the entity
        """
        return wrap.entity_has_signal(self.obj, name)

    def displaySignals(self):
        """
        Print the list of signals into standard output: temporary.
        """
        signals = list(self.signals())
        if len(signals) == 0:
            display("--- <" + self.name + "> has no signal")
        else:
            display("--- <" + self.name + "> signal list: ")
            for s in signals[:-1]:
                display("    |-- <" + str(s))
            display("    `-- <" + str(signals[-1]))

    def signals(self):
        """
        Return the list of signals
        """
        sl = wrap.entity_list_signals(self.obj)
        return map(lambda pyObj: signal_base.SignalBase(obj=pyObj), sl)

    def commands(self):
        """
        Return the list of commands.
        """
        return wrap.entity_list_commands(self.obj)

    def globalHelp(self):
        """
        Print a short description of each command.
        """
        if self.__doc__:
            print(self.__doc__)
        print("List of commands:")
        print("-----------------")
        for cstr in self.commands():
            ctitle = cstr + ':'
            for i in range(len(cstr), 15):
                ctitle += ' '
            for docstr in wrap.entity_get_command_docstring(self.obj, cstr).split('\n'):
                if (len(docstr) > 0) and (not docstr.isspace()):
                    display(ctitle + "\t" + docstr)
                    break

    def help(self, comm=None):
        """
        With no arg, print the global help. With arg the name of
        a specific command, print the help associated to the command.
        """
        if comm is None:
            self.globalHelp()
        else:
            display(comm + ":\n" + wrap.entity_get_command_docstring(self.obj, comm))

    def __getattr__(self, name):
        try:
            return self.signal(name)
        except Exception:
            try:
                object.__getattr__(self, name)
            except AttributeError:
                raise AttributeError("'%s' entity has no attribute %s\n" % (self.name, name) +
                                     '  entity attributes are usually either\n' + '    - commands,\n' +
                                     '    - signals or,\n' + '    - user defined attributes')

    def __setattr__(self, name, value):
        if name in map(lambda s: s.getName().split(':')[-1], self.signals()):
            raise NameError(name + " already designates a signal. "
                            "It is not advised to set a new attribute of the same name.")
        object.__setattr__(self, name, value)

    # --- COMMANDS BINDER -----------------------------------------------------
    # List of all the entity classes from the c++ factory, that have been bound
    # bind the py factory.
    entityClassNameList = []

    # This function dynamically create the function object that runs the command.
    @staticmethod
    def createCommandBind(name, docstring):
        def commandBind(self, *arg):
            return wrap.entity_execute_command(self.obj, name, arg)

        commandBind.__doc__ = docstring
        return commandBind

    def boundClassCommands(self):
        """
        This static function has to be called from a class heritating from Entity.
        It should be called only once. It parses the list of commands obtained from
        c++, and bind each of them to a python class method.
        """
        # Get list of commands of the Entity object
        commands = wrap.entity_list_commands(self.obj)
        # for each command, add a method with the name of the command
        for cmdstr in commands:
            docstr = wrap.entity_get_command_docstring(self.obj, cmdstr)
            cmdpy = Entity.createCommandBind(cmdstr, docstr)
            setattrpath(self.__class__, cmdstr, cmdpy)

    def boundNewCommand(self, cmdName):
        """
        At construction, all existing commands are bound directly in the class.
        This method enables to bound new commands dynamically. These new bounds
        are not made with the class, but directly with the object instance.
        """
        if (cmdName in self.__dict__) | (cmdName in self.__class__.__dict__):
            print("Warning: command ", cmdName, " will overwrite an object attribute.")
        docstring = wrap.entity_get_command_docstring(self.obj, cmdName)
        cmd = Entity.createCommandBind(cmdName, docstring)
        # Limitation (todo): does not handle for path attribute name (see setattrpath).
        setattr(self, cmdName, types.MethodType(cmd, self))

    def boundAllNewCommands(self):
        """
        For all commands that are not attribute of the object instance nor of the
        class, a new attribute of the instance is created to bound the command.
        """
        cmdList = wrap.entity_list_commands(self.obj)
        cmdList = filter(lambda x: x not in self.__dict__, cmdList)
        cmdList = filter(lambda x: x not in self.__class__.__dict__, cmdList)
        for cmd in cmdList:
            self.boundNewCommand(cmd)

    def setLoggerVerbosityLevel(self, verbosity):
        """
        Specify for the entity the verbosity level
        """
        return wrap.entity_set_logger_verbosity(self.obj, verbosity)

    def getLoggerVerbosityLevel(self):
        """
        Returns the entity's verbosity level
        """
        r = wrap.entity_get_logger_verbosity(self.obj)
        if r == 0:
            return VerbosityLevel.VERBOSITY_ALL
        elif r == 1:
            return VerbosityLevel.VERBOSITY_INFO_WARNING_ERROR
        elif r == 2:
            return VerbosityLevel.VERBOSITY_WARNING_ERROR
        elif r == 3:
            return VerbosityLevel.VERBOSITY_ERROR
        return VerbosityLevel.VERBOSITY_NONE

    def setTimeSample(self, timeSample):
        """
        Specify for the entity the time at which call is counted.
        """
        return wrap.entity_set_time_sample(self.obj, timeSample)

    def getTimeSample(self):
        """
        Returns for the entity the time at which call is counted.
        """
        return wrap.entity_get_time_sample(self.obj)

    def setStreamPrintPeriod(self, streamPrintPeriod):
        """
        Specify for the entity the period at which debugging information is printed
        """
        return wrap.entity_set_stream_print_period(self.obj, streamPrintPeriod)

    def getStreamPrintPeriod(self):
        """
        Returns for the entity the period at which debugging information is printed
        """
        return wrap.entity_get_stream_print_period(self.obj)
