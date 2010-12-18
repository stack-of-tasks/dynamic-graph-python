"""
Copyright (c) 2010 CNRS
Author: Florent Lamiraux
"""

import sys, dl
flags = sys.getdlopenflags()
# Import C++ symbols in a global scope
# This is necessary for signal compiled in different modules to be compatible
sys.setdlopenflags(dl.RTLD_NOW|dl.RTLD_GLOBAL)
from wrap import *
# Recover previous flags
sys.setdlopenflags(flags)

import entity, signal_base
import re

def plug (signalOut, signalIn) :
    """
    Plug an output signal into an input signal
    """
    # get signals and entities
    w_plug(signalOut.obj, signalIn.obj)
