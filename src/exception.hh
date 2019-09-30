// Copyright 2010, Florent Lamiraux, Thomas Moulard, LAAS-CNRS.
//
// This file is part of dynamic-graph-python.
// dynamic-graph-python is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either
// version 3 of the License, or (at your option) any later version.
//
// dynamic-graph-python is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Lesser Public License for more details.  You should
// have received a copy of the GNU Lesser General Public License along
// with dynamic-graph. If not, see <http://www.gnu.org/licenses/>.

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
