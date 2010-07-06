"""
Copyright (c) 2010 CNRS
Author: Florent Lamiraux
"""

from wrap import *
import entity, signal_base
import re

def plug (signalIn, signalOut) :
    """
    Plug an output signal into an input signal
    syntax is plug ("entityIn.signalIn", "entityOut.signalOut")
    """
    # get signals and entities
    [eIn, sIn] = re.split("\.", signalOut)
    [eOut, sOut] = re.split("\.", signalOut)
    w_plug(eIn, sIn, eOut, sOut)
