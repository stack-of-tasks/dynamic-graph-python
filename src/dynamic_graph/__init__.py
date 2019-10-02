"""
Copyright (c) 2010 CNRS
Author: Florent Lamiraux
"""

import sys

from . import entity  # noqa
from . import signal_base  # noqa
from .wrap import *  # noqa

try:
    from DLFCN import RTLD_NOW, RTLD_GLOBAL
except ModuleNotFoundError:  # Python 3
    from os import RTLD_NOW, RTLD_GLOBAL

flags = sys.getdlopenflags()

# Import C++ symbols in a global scope
# This is necessary for signal compiled in different modules to be compatible

sys.setdlopenflags(RTLD_NOW | RTLD_GLOBAL)

# Recover previous flags
sys.setdlopenflags(flags)


def plug(signalOut, signalIn):
    """
    Plug an output signal into an input signal
    """
    # get signals and entities
    w_plug(signalOut.obj, signalIn.obj)  # noqa
