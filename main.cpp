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
