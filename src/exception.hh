// Copyright 2010, Florent Lamiraux, Thomas Moulard, LAAS-CNRS.

#ifndef DYNAMIC_GRAPH_PYTHON_EXCEPTION
#define DYNAMIC_GRAPH_PYTHON_EXCEPTION

/// \brief Catch all exceptions which may be sent when C++ code is
/// called.
#define CATCH_ALL_EXCEPTIONS()                       \
  catch (const std::exception& exc) {                \
    PyErr_SetString(dgpyError, exc.what());          \
    return NULL;                                     \
  }                                                  \
  catch (const char* s) {                            \
    PyErr_SetString(dgpyError, s);                   \
    return NULL;                                     \
  }                                                  \
  catch (...) {                                      \
    PyErr_SetString(dgpyError, "Unknown exception"); \
    return NULL;                                     \
  }                                                  \
  struct e_n_d__w_i_t_h__s_e_m_i_c_o_l_o_n

#endif  //! DYNAMIC_GRAPH_PYTHON_EXCEPTION
