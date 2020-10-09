# params-concept
This code is a concept implementation of an extremely lightweight parameter parsing library for c++.
My main annoyance with other parameter parsing libraries such as boost program options is that they treat the set of command line parameters as dynamic when in almost all cases they are actually statically known.
I typically find my code filled with lines such as `int parameter = p["parameter_name"].as<int>();` which feels very wasteful since everybody besides the compiler already knowns that the parameter is always an `int`.
This library provides a macro `DEFINE_PARAMETER_STRUCT` which facilitates parsing command line arguments into a plain structure.
It relies on `boost::hana` for introspection and `boost::lexical_cast` for conversions.

# Installation
All that is needed to use `params-concept` is to include the single header file `params.hpp`. `params-concept` depends on boost, specifically `boost::hana` and `boost:lexical_cast`.

# Usage
The following short program demonstrates how to use `params-concept`.
```
#include "params.hpp"

// takes name and list of tuples of form (type, name, default, help)
DEFINE_PARAMETERS_STRUCT(Parameters, (int, i, 0, "integer"),
                         (std::string, name, "output", "output file"),
                         (bool, help, false, "print help")
                         );

int main(int argc, char *argv[]) {
  Parameters p;

  try {
    parse_params(argc, argv, p);
  } catch (std::exception& e) {
    std::cout << "error: " << e.what() << "\n";
    return -1;
  }

  if (p.help) {
    print_desc(std::cout, p);
    return -1;
  }

  print_values(std::cout, p);

  return 0;
}
```
