"""
Copyright (c) 2010 CNRS
Author: Florent Lamiraux
"""

from wrap import *
import entity, signal_base
import re

def plug (signalOut, signalIn) :
    """
    Plug an output signal into an input signal
    """
    # get signals and entities
    w_plug(signalOut.obj, signalIn.obj)
