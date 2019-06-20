# numpy causes troubles when Py_Finalize is called in Interpreter destructor.
import numpy

# Make sure the variable is deleted.
if "var" in locals() or "var" in globals():
    raise ValueError('Not cleaned')

var = "This string should have been deleted."
