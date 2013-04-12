// Copyright 2011, Florent Lamiraux CNRS.
//
// This file is part of dynamic-graph-python.
// dynamic-graph is free software: you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// dynamic-graph is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Lesser Public License for more details.  You should have
// received a copy of the GNU Lesser General Public License along with
// dynamic-graph. If not, see <http://www.gnu.org/licenses/>.

#include <sstream>
#include <string>
#include <vector>
#include <list>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>

#include <dynamic-graph/debug.h>

#include "dynamic-graph/python/interpreter.hh"

using namespace dynamicgraph;

// Factorize exception catching code.
#define CATCH_EXCEPTIONS()						\
  catch (std::exception& e)						\
    {									\
      std::cout								\
	<< errorPrefix							\
	<< e.what () << std::endl;					\
    }									\
  catch (const char* str)						\
    {									\
      std::cout << errorPrefix						\
		<< "Unknown exception " << str << std::endl;		\
    }									\
  catch (...)								\
    {									\
      dgDEBUG(5) << errorPrefix << " Unknown exception " << std::endl;	\
    }									\
  struct e_n_d__w_i_t_h__s_e_m_i_c_o_l_o_n

struct Options
{
  /// \brief Prologue path (a prologue is a script implicitly
  /// evaluated at start-up).
};

int main (int argc, char** argv)
{
  dgDEBUGIN(15);

  dgDEBUG(5) << " Loading..." << std::endl;

  // Parse options.
  std::vector<std::string> inputs;
  namespace po = boost::program_options;
  po::options_description desc ("Allowed options");
  desc.add_options ()
    ("input,i",
     po::value<std::vector<std::string> >(&inputs),
     "file(s) evaluated at start-up")
    ("help,h", "produce help message")
    ;

  po::positional_options_description p;
  p.add("input", -1);

  po::variables_map vm;

  try
    {
      po::store(po::command_line_parser(argc, argv).
		options(desc).positional(p).run(), vm);
      po::notify(vm);

      if (vm.count ("help"))
	{
	  std::cout << "Usage: " << argv[0] << " [options]" << std::endl
		    << desc << std::endl
		    << "Report bugs to <hpp@laas.fr>" << std::endl;
	  return 1;
	}
    }
  catch (po::error& error)
    {
      std::cerr << "Error while parsing argument: "
		<< error.what () << std::endl;
      return 1;
    }

  std::list<std::string> inputList (inputs.begin (),
				    inputs.end ());

  // Load all input files.
  dynamicgraph::python::Interpreter interpreter;
  BOOST_FOREACH (const std::string& pathStr, inputList)
    {
      boost::filesystem::path path (pathStr);

      std::stringstream ss;
#if BOOST_FILESYSTEM_VERSION == 2
      ss << "!! In file <" << path.file_string () << "> : ";
#else 
      ss << "!! In file <" << path.string () << "> : ";
#endif
      std::string errorPrefix = ss.str ();

      try
	{
#if BOOST_FILESYSTEM_VERSION == 2
	  interpreter.runPythonFile (path.file_string ());
#else
          interpreter.runPythonFile (path.string ());
#endif
	}
      CATCH_EXCEPTIONS ();
      return 0;
    }

  std::string errorPrefix = "";
  std::string command;
  while(1) {
    command = interpreter.processStream(std::cin, std::cout);
    if (command != "\n") {
      std::string result, out, err;
      interpreter.python(command, result, out, err);

      if (out != "")  {
	std::cout << out << std::endl;
      }
      if (err != "")  {
	std::cout << err << std::endl;
      }
      if (result != "None")  {
	std::cout << result << std::endl;
      }
    }
    if (std::cin.eof ())
      break;
  }
  std::cout << std::endl;
  return 0;
}
