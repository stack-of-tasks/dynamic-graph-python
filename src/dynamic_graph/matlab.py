# The matlab class is define to produce a nice rendering of vector and matrix
# signals. The class builds at construction a string from the display
# of the matrix/vector, and stores the string internally. The string can be recovered
# using the str() member.
# The statics prec, space and fullPrec can be used to modify the display.

from __future__ import print_function


def pseudozero(prec):
    """
  Return a string with '0...' and enough space to fill prec cars.
  """
    res = '0...'
    for i in range(2, prec):
        res += ' '
    return res


class matlab:
    prec = 12
    space = 2
    fullPrec = 1e-5

    def __init__(self, obj):
        try:
            return self.matlabFromMatrix(obj)
        except Exception:
            pass
        try:
            return self.matlabFromVector(obj)
        except Exception:
            pass
        self.resstr = str(obj)

    def __str__(self):
        return self.resstr

    def matlabFromMatrix(self, A):
        fm = '%.' + str(self.prec) + 'f'
        maxstr = 0
        mstr = (())
        for v in A:
            lnstr = ()
            for x in v:
                if (abs(x) < self.fullPrec * self.fullPrec):
                    curr = '0'
                else:
                    if (abs(x) < self.fullPrec):
                        curr = pseudozero(self.prec)
                    else:
                        curr = ' ' + (fm % x)
                if (maxstr < len(curr)):
                    maxstr = len(curr)
                lnstr += (curr, )
            mstr += (lnstr, )

        maxstr += self.space
        resstr = '[...\n'
        first = True
        for v in mstr:
            if first:
                first = False
            else:
                resstr += ' ;\n'
            firstC = True
            for x in v:
                if firstC:
                    firstC = False
                else:
                    resstr += ','
                for i in range(1, maxstr - len(x)):
                    resstr += ' '
                resstr += x
        resstr += '   ];'
        self.resstr = resstr

    def matlabFromVector(self, v):
        fm = '%.' + str(self.prec) + 'f'
        maxstr = 0
        vstr = (())
        for x in v:
            if (abs(x) < self.fullPrec * self.fullPrec):
                curr = '0'
            else:
                if (abs(x) < self.fullPrec):
                    curr = pseudozero(self.prec)
                else:
                    curr = ' ' + (fm % x)
            if (maxstr < len(curr)):
                maxstr = len(curr)
            vstr += (curr, )

        maxstr += self.space
        resstr = '[  '
        first = True
        for x in vstr:
            if first:
                first = False
            else:
                resstr += ','
            for i in range(1, maxstr - len(x)):
                resstr += ' '
            resstr += x
        resstr += '   ]\';'
        self.resstr = resstr
