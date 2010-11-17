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
    syntax is plug ("entityIn.signalIn", "entityOut.signalOut")
    """
    # get signals and entities
    [eOut, sOut] = re.split("\.", signalOut)
    [eIn, sIn] = re.split("\.", signalIn)
    w_plug(eOut, sOut, eIn, sIn)
