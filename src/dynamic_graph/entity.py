"""
  Copyright (C) 2010 CNRS

  Author: Florent Lamiraux
"""
import wrap, signal_base

entityClassNameList = []

def commandMethod(name, docstring) :
    def method(self, *arg):
        return wrap.entity_execute_command(self.obj, name, arg)
    method.__doc__ = docstring
    return method

def initEntity(self, name):
    """
    Common constructor of Entity classes
    """
    Entity.__init__(self, self.className, name)
    if not self.__class__.commandCreated:
        # Get list of commands of the Entity object
        commands = wrap.entity_list_commands(self.obj)
        # for each command, add a method with the name of the command
        for command in commands:
            docstring = wrap.entity_get_command_docstring(self.obj, command)
            setattr(self.__class__, command, commandMethod(command, docstring))
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

    obj = None

    def __init__(self, className, instanceName):
        """
        Constructor: if not called by a child class, create and store a pointer
        to a C++ Entity object.
        """
        object.__setattr__(self, 'obj', wrap.create_entity(className, instanceName) )

    @property
    def name(self) :
        return wrap.entity_get_name(self.obj)

    def signal (self, name) :
        """
        Get a signal of the entity from signal name
        """
        signalPt = wrap.entity_get_signal(self.obj, name)
        return signal_base.SignalBase(name = "", obj = signalPt)

    def displaySignals(self) :
        """
        Print the list of signals into standard output: temporary.
        """
        signals = self.signals()
        print "--- <" +  self.name + "> signal list: "
        for s in signals[:-1]:
            print "    |-- <" + str(s)
        print "    `-- <" + str(signals[-1])

    def signals(self) :
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
        for cstr in self.commands():
            ctitle=cstr+':'
            for i in range(len(cstr),15):
                ctitle+=' '
            for docstr in wrap.entity_get_command_docstring(self.obj,cstr).split('\n'):
                if (len(docstr)>0) and (not docstr.isspace()):
                    print ctitle+"\t"+docstr
                    break

    def help( self,comm=None ):
        """
        With no arg, print the global help. With arg the name of
        a specific command, print the help associated to the command.
        """
        if comm is None:
            self.globalHelp()
        else:
            print comm+":\n"+wrap.entity_get_command_docstring(self.obj,comm)


    def __getattr__(self, name):
        try:
            return self.signal(name)
        except:
            object.__getattr__(self, name)

    # Script short-cuts: don't use this syntaxt in python coding,
    # use it for debuging online only!
    @property
    def sigs(self):
        self.displaySignals()
