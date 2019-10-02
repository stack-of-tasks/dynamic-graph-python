"""
  Copyright (C) 2010 CNRS

  Author: Florent Lamiraux
"""
import re

from .wrap import (create_signal_wrapper, signal_base_display, signal_base_display_dependencies,
                   signal_base_get_class_name, signal_base_get_name, signal_base_get_time, signal_base_get_value,
                   signal_base_getPlugged, signal_base_isPlugged, signal_base_recompute, signal_base_set_time,
                   signal_base_set_value, signal_base_unplug)


def stringToTuple(vector):
    """
    Transform a string of format '[n](x_1,x_2,...,x_n)' into a tuple of numbers.
    """
    # Find vector length
    a = re.match(r'\[(\d+)\]', vector)
    size = int(a.group(1))
    # remove '[n]' prefix
    vector = vector[len(a.group(0)):]
    # remove '(' and ')' at beginning and end
    vector = vector.lstrip('(').rstrip(')\n')
    # split string by ','
    vector = vector.split(',')
    # check size
    if len(vector) != size:
        raise TypeError('displayed size ' + str(size) + ' of vector does not fit actual size: ' + str(len(vector)))
    res = map(float, vector)
    return tuple(res)


def tupleToString(vector):
    """
    Transform a tuple of numbers into a string of format
    '[n](x_1, x_2, ..., x_n)'
    """
    string = '[%d](' % len(vector)
    for x in vector[:-1]:
        string += '%f,' % x
    string += '%f)' % vector[-1]
    return string


def stringToMatrix(string):
    """
    Transform a string of format
    '[n,m]((x_11,x_12,...,x_1m),...,(x_n1,x_n2,...,x_nm))' into a tuple
    of tuple of numbers.
    """
    # Find matrix size
    a = re.search(r'\[(\d+),(\d+)]', string)
    nRows = int(a.group(1))
    nCols = int(a.group(2))
    # Remove '[n,m]' prefix
    string = string[len(a.group(0)):]
    rows = string.split('),(')
    if len(rows) != nRows:
        raise TypeError('displayed nb rows ' + nRows + ' of matrix does not fit actual nb rows: ' + str(len(rows)))
    m = []
    for rstr in rows:
        rstr = rstr.lstrip('(').rstrip(')\n')
        r = map(float, rstr.split(','))
        if len(r) != nCols:
            raise TypeError('one row length ' + len(r) + ' of matrix does not fit displayed nb cols: ' + nCols)
        m.append(tuple(r))
    return tuple(m)


def matrixToString(matrix):
    """
    Transform a tuple of tuple of numbers into a string of format
    '[n,m]((x_11,x_12,...,x_1m),...,(x_n1,x_n2,...,x_nm))'.
    """
    nRows = len(matrix)
    if nRows == 0:
        return '[0,0](())'
    nCols = len(matrix[0])
    string = '[%d,%d](' % (nRows, nCols)
    for r in range(nRows):
        string += '('
        for c in range(nCols):
            string += str(float(matrix[r][c]))
            if c != nCols - 1:
                string += ','
        string += ')'
        if r != nRows - 1:
            string += ','
    string += ')'
    return string


def objectToString(obj):
    """
    Transform an object to a string. Object is either
      - an entity (more precisely a sub-class named Feature)
      - a matrix
      - a vector or
      - a floating point number,
      - an integer,
      - a boolean,
    """
    if (hasattr(obj, "__iter__")):
        # matrix or vector
        if len(obj) == 0:
            return ""
        else:
            if (hasattr(obj[0], "__iter__")):
                # matrix
                return matrixToString(obj)
            else:
                # vector
                return tupleToString(obj)
    elif hasattr(obj, 'name'):
        return obj.name
    else:
        return str(obj)


def stringToObject(string):
    """
    Convert a string into one of the following types
      - a matrix (tuple of tuple),
      - a vector,
      - an integer,
      - a floating point number.
    Successively attempts conversion in the above order and return
    on success. If no conversion fits, the string is returned.
    """
    if isinstance(string, float):
        return string
    if isinstance(string, int):
        return string
    if isinstance(string, tuple):
        return string
    try:
        return stringToMatrix(string)
    except Exception:
        pass
    try:
        return stringToTuple(string)
    except Exception:
        pass
    try:
        return int(string)
    except Exception:
        pass
    try:
        return float(string)
    except Exception:
        return string


class SignalBase(object):
    """
    This class binds dynamicgraph::SignalBase<int> C++ class
    """

    obj = None

    def __init__(self, name="", obj=None):
        """
        Constructor: if not called by a child class, create and store a pointer
        to a C++ SignalBase<int> object.
        """
        if obj:
            self.obj = obj
        else:
            raise RuntimeError("A pointer is required to create SignalBase object.")

        if obj is None:
            self.className = self.getClassName()
            self.name = self.getName()

    @property
    def time(self):
        """
        Get time of signal
        """
        return signal_base_get_time(self.obj)

    @time.setter
    def time(self, val):
        """
        Set Time of signal

          Input:
            - an integer
        """
        return signal_base_set_time(self.obj, val)

    @property
    def value(self):
        """
        Setter and getter for the value of a signal

        Binds C++ SignalBase<int>::get() and set() methods. Values are passed
        through string streams.
        A string is interpreted as respectively:
        * a matrix (tuple of tuple) if string fits '[n,m]((x_11,x_12,...,x_1m),...,(x_n1,x_n2,...,x_nm))' format where
          n and m are integers, x_ij are floating point numbers,
        * a tuple if string fits '[n](x_1, x_2, ..., x_n)' format,
        * an integer,
        * a floating point number.

        If string fits none of the above formats, no conversion is performed.

        For instance, is s binds a signal of type vector,
        >>> s.value = (2.5, .1, 1e2)
        will call SignalBase<int>::set("[3](2.5,0.1,100.0)") and
        >>> s.value
        (2.5, 0.1, 100.0)
        """
        string = signal_base_get_value(self.obj)
        return stringToObject(string)

    @value.setter
    def value(self, val):
        """
        Set the signal as a constant signal with given value.
        If the signal is plugged, it will be unplugged
        """
        string = objectToString(val)
        return signal_base_set_value(self.obj, string)

    def getName(self):
        """
        Get name of signal
        """
        return signal_base_get_name(self.obj)

    @property
    def name(self):
        """
        Get name of signal
        """
        return signal_base_get_name(self.obj)

    def getClassName(self):
        """
        Get class name of signal
        """
        return signal_base_get_class_name(self.obj)

    def recompute(self, time):
        """
        Force signal to recompute the value at given time.
        """
        return signal_base_recompute(self.obj, time)

    def unplug(self):
        """
        Unplug a PTR signal.
        """
        return signal_base_unplug(self.obj)

    def isPlugged(self):
        """
        Return whether a signal is plugged.
        """
        return signal_base_isPlugged(self.obj)

    def getPlugged(self):
        """
        Return the plugged signal.
        """
        return SignalBase(obj=signal_base_getPlugged(self.obj))

    def __str__(self):
        """
        Print signal in a string
        """
        return signal_base_display(self.obj)

    def displayDependencies(self, iter):
        """
        Print signal dependencies in a string
        """
        return (signal_base_display_dependencies(self.obj, iter))


class SignalWrapper(SignalBase):
    def __init__(self, name, type, func):
        super(SignalWrapper, self).__init__(name, create_signal_wrapper(name, type, func))
