// Copyright 2010, Florent Lamiraux, Thomas Moulard, LAAS-CNRS.

#ifndef DYNAMIC_GRAPH_PYTHON_EXCEPTION
#define DYNAMIC_GRAPH_PYTHON_EXCEPTION

#include "dynamic-graph/python/dynamic-graph-py.hh"

/// \brief Catch all exceptions which may be sent when C++ code is
/// called.
#define CATCH_ALL_EXCEPTIONS()                       \
  catch (const std::exception& exc) {                \
    PyErr_SetString(DGPYERROR, exc.what());          \
    return NULL;                                     \
  }                                                  \
  catch (const char* s) {                            \
    PyErr_SetString(DGPYERROR, s);                   \
    return NULL;                                     \
  }                                                  \
  catch (...) {                                      \
    PyErr_SetString(DGPYERROR, "Unknown exception"); \
    return NULL;                                     \
  }                                                  \
  struct e_n_d__w_i_t_h__s_e_m_i_c_o_l_o_n

#endif  //! DYNAMIC_GRAPH_PYTHON_EXCEPTION
