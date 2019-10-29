"""
Copyright (c) 2010 CNRS
Author: Florent Lamiraux
"""

import sys

import DLFCN
import entity  # noqa
import signal_base  # noqa

# Import C++ symbols in a global scope
# This is necessary for signal compiled in different modules to be compatible
flags = sys.getdlopenflags()
sys.setdlopenflags(DLFCN.RTLD_NOW | DLFCN.RTLD_GLOBAL)
from wrap import *  # noqa
# Recover previous flags
sys.setdlopenflags(flags)


def plug(signalOut, signalIn):
    """
    Plug an output signal into an input signal
    """
    # get signals and entities
    w_plug(signalOut.obj, signalIn.obj)  # noqa
