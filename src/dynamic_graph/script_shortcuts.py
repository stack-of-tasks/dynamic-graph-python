# Defines the following shortcuts:
# signal.name -> return the shortname
# signal      -> display nicely the content of the signal
# signal(3)   -> recompute the signal at time 3, and display nicely
# signal +1   -> increment the signal time of 1, recompute, and display.
# signal.deps -> display the graph dependancy up to the default value (3)
# signal.deps(6) -> same, but with depth = 6.
# entity      -> same as print(entity)
# change the prompt to be '%'

from __future__ import print_function

# Changing prompt
import sys

from .entity import Entity
from .signal_base import SignalBase


# Enables shortcut "name"
def sig_short_name(self):
    return self.getName().split(':')[-1]


setattr(SignalBase, 'name', property(sig_short_name))


# Enables shortcuts "m"
# This code implements a pseudo function 'm' in the class signal_base,
# with no args, or optional args. Three calls can be made:
#    - sig.m : print the current value.
#    - sig.m(time): recompute at given <time>, and display the current value
#    - sig.m +time: recompute at <time> after current time, and display.
class PrettySignalPrint:
    sig = None

    def __init__(self, sig):
        self.sig = sig

    def __str__(self):
        return self.sig.name + " = " + str(self.sig.value)

    def __repr__(self):
        return str(self)

    def __call__(self, iter):
        self.sig.recompute(iter)
        return self

    def __add__(self, iter):
        self.sig.recompute(self.sig.time + iter)
        return self


def sigMatPrint(sig):
    return PrettySignalPrint(sig)


setattr(SignalBase, 'm', property(PrettySignalPrint))


# Enable the same as 'm', but directly on the signal object.
def sigRepr(self):
    return self.name + ' = ' + str(self.value)


def sigCall(sig, iter):
    sig.recompute(iter)
    print(sigRepr(sig))


def sigTimeIncr(sig, iter):
    sig.recompute(sig.time + iter)
    print(sigRepr(sig))


setattr(SignalBase, '__repr__', sigRepr)
setattr(SignalBase, '__call__', sigCall)
setattr(SignalBase, '__add__', sigTimeIncr)


# Enables shortcut "deps"
# Implements the peudo function 'deps', that can be called without arg,
# or specifying a specific depth to be printed.
class SignalDepPrint:
    defaultDepth = 2
    sig = None

    def __init__(self, sig):
        self.sig = sig

    def __repr__(self):
        return self.sig.displayDependencies(self.defaultDepth)

    def __call__(self, depth):
        self.defaultDepth = depth
        return self


setattr(SignalBase, 'deps', property(SignalDepPrint))

setattr(Entity, 'sigs', property(Entity.displaySignals))
setattr(Entity, '__repr__', Entity.__str__)

sys.ps1 = '% '


# Enable function that can be call without()def optionalparentheses(f):
def optionalparentheses(f):

    class decoclass:

        def __init__(self, f):
            self.functor = f

        def __repr__(self):
            res = self.functor()
            if isinstance(res, str):
                return res
            else:
                return ''

        def __call__(self, *arg):
            return self.functor(*arg)

    return decoclass(f)
