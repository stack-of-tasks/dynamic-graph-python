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
    syntax is plug ("entityOut.signalOut", "entityIn.signalIn")
    """
    # get signals and entities
    [eOut, sOut] = re.split("\.", signalOut)
    [eIn, sIn] = re.split("\.", signalOut)
    w_plug(eOut, sOut, eIn, sIn)
