"""
  Copyright (C) 2010 CNRS

  Author: Florent Lamiraux
"""
import wrap
import re

def stringToTuple (vector) :
    """
    Transform a string of format '[n](x_1,x_2,...,x_n)' into a tuple of numbers.
    """
    # Find vector length
    a = re.match('\[(\d+)\]',vector)
    size = int(a.group(1))
    format = '\('
    for i in range(size):
        format += '(.*)'
        if i != size-1:
            format += ','
    format += '\)'
    a = re.search(format, vector)
    res = []
    for i in range (1, size+1):
        res.append (float(a.group(i)))
    return tuple (res)

def tupleToString (vector) :
    """
    Transform a tuple of numbers into a string of format
    '[n](x_1, x_2, ..., x_n)'
    """
    string = '[%d]('%len (vector)
    for x in vector[:-1]:
        string += '%f,'%x
    string += '%f)'%vector[-1]
    return string

def stringToMatrix (string) :
    """
    Transform a string of format
    '[n,m]((x_11,x_12,...,x_1m),...,(x_n1,x_n2,...,x_nm))' into a tuple
    of tuple of numbers.
    """
    # Find matrix size
    a = re.search ('\[(\d+),(\d+)]', string)
    nRows = int (a.group(1))
    nCols = int (a.group(2))
    format = '\('
    for row in range (nRows) :
        format += '\('
        for col in range (nCols) :
            format += '(.*)'
            if col != nCols-1 :
                format += ','
        format += '\)'
        if row != nRows-1 :
            format += ','
    format += '\)'
    a = re.search (format, string)
    m = []
    index = 1
    for r in range (nRows) :
        row = []
        for c in range (nCols) :
            row.append (float (a.group (index)))
            index += 1
        m.append (tuple (row))
    return tuple (m)

def matrixToString(matrix) :
    """
    Transform a tuple of tuple of numbers into a string of format
    '[n,m]((x_11,x_12,...,x_1m),...,(x_n1,x_n2,...,x_nm))'.
    """
    nRows = len(matrix)
    if nRows is 0 :
        return '[0,0](())'
    nCols = len(matrix[0])
    string = '[%d,%d](' % (nRows, nCols)
    for r in range (nRows) :
        string += '('
        for c in range (nCols) :
            string += str (float (matrix [r][c]))
            if c != nCols-1 :
                string += ','
        string += ')'
        if r != nRows -1 :
            string += ','
    string += ')'
    return string

def objectToString(obj) :
    """
    Transform an object to a string. Object is either
      - a floating point number,
      - an integer,
      - a boolean,
      - a vector or
      - a matrix
    """
    if (isinstance(obj, tuple)) :
        # matrix or vector
        if len(obj) is 0 :
            return ""
        else :
            if (isinstance(obj[0], tuple)) :
                #matrix
                return matrixToString(obj)
            else :
                #vector
                return tupleToString(obj)
    else :
        return str(obj)

def stringToObject(string) :
    """
    Convert a string into one of the following types
      - a matrix (tuple of tuple),
      - a vector,
      - an integer,
      - a floating point number.
    Successively attempts conversion in the above order and return
    on success. If no conversion fits, the string is returned.
    """
    try :
        return stringToMatrix(string)
    except :
        pass
    try :
        return stringToTuple(string)
    except :
        pass
    try :
        return int(string)
    except :
        pass
    try :
        return float(string)
    except :
        return string

class SignalBase (object) :
    """
    This class binds dynamicgraph::SignalBase<int> C++ class
    """

    object = None

    def __init__(self, name, object = None) :
        """
        Constructor: if not called by a child class, create and store a pointer
        to a C++ SignalBase<int> object.
        """
        if object :
            self.object = object
        if not self.object :
            self.object = wrap.create_signal_base(self, name)

    @property
    def time(self) :
        """
        Get time of signal
        """
        return wrap.signalBaseGetTime(self.object)

    @property
    def value(self) :
        """
        Read the value of a signal
        """
        string = wrap.signal_base_get_value(self.object)
        return stringToObject(string)

    @value.setter
    def value(self, val) :
        """
        Set the signal as a constant signal with given value.
        If the signal is plugged, it will be unplugged
        """
        string = objectToString(val)
        return wrap.signal_base_set_value(self.object, string)
