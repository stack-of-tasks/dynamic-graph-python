# Copyright (C) 2020 CNRS
#
# Author: Joseph Mirabel

from __future__ import print_function

from .wrap import SignalBase, create_signal_wrapper as SignalWrapper

# I kept what follows for backward compatibility but I think it should be
# removed
import re

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
