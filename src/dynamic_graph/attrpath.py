# This module define the three functions:
#   - getattrpath
#   - setattrpath
#   - existattrpath
# that work similarly as the get/setattr, but given path ("a.b.c.d")
# in input. Consider the following example:
#  >>> setattrpath( e.__class__,"a.b.c.d",fun)
# with fun a function (self,*arg). Then, it is next possible to launch
#  >>> e.a.b.c.d( ...)
# as if it was a classical member function of e.


class CommandPath(object):
    """
    This class is only defined to implement a path of attribute
    to store entity commands. It has no members except those automatically
    defined at run time (which should be CommandPath or functions).
    """
    mother = None

    def __getattr__(self, name):
        privateName = name + '_obj'
        if privateName in self.__dict__:
            obj = getattr(self, privateName)
            obj.mother = self.mother
            return obj
        return object.__getattr__(self, name)


def createCommandModule(target, name):
    def createGetter(name):
        def __(self):
            obj = getattr(self, name)
            obj.mother = self
            return obj

        return __

    privateName = name + '_obj'
    setattr(target, privateName, CommandPath())
    # module = getattr(target, privateName)

    if not isinstance(target, CommandPath):
        setattr(target, name, property(createGetter(privateName)))


class CommandLauncher(object):
    """
    """
    mother = None
    fun = None

    def __init__(self, fun):
        self.fun = fun

    def __call__(self, *arg):
        return self.fun(self.mother, *arg)


def createCommandLauncher(target, name, fun):
    if isinstance(target, CommandPath):
        privateName = name + '_obj'
        setattr(target, privateName, CommandLauncher(fun))
    else:
        setattr(target, name, fun)


def setattrpath(target, path, attribute):
    """
    Create in target an new attribute with value path (available at
    target.path1. ... .pathn).
    """
    pathk = target
    read = True
    if isinstance(path, str):
        path = path.split('.')
    for tokenk in path[0:-1]:
        if (not read) | (tokenk not in pathk.__dict__):
            read = False
            createCommandModule(pathk, tokenk)
        pathk = getattr(pathk, tokenk + "_obj")
    if callable(attribute):
        createCommandLauncher(pathk, path[-1], attribute)
    else:
        print("Should not happen")
        setattr(pathk, path[-1], attribute)


def getattrpath(target, path):
    """
    Get in target the value located at path (available at
    target.path1. ... .pathn).
    """
    pathk = target
    if isinstance(path, str):
        path = path.split('.')
    for tokenk in path:
        privateName = tokenk + "_obj"
        if privateName in pathk.__dict__:
            pathk = getattr(pathk, privateName)
        else:
            if tokenk in pathk.__dict__:
                pathk = getattr(pathk, tokenk)
            else:
                raise Exception('Path does not exist -- while accessing "' + tokenk + '" in ' + '.'.join(path))
    return pathk


def existattrpath(target, path):
    """
    Check for the existence in target of a value located at path (available at
    target.path1. ... .pathn).
    """
    pathk = target
    if isinstance(path, str):
        path = path.split('.')
    for tokenk in path[0:-1]:
        print('check ', tokenk)
        privateName = tokenk + "_obj"
        if (privateName not in pathk.__dict__):
            return False
        pathk = getattr(pathk, privateName)
    name = path[-1]
    privateName = name + "_obj"
    return (name in pathk.__dict__) | (privateName in pathk.__dict__)
