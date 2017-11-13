#include "params.hpp"

DEFINE_PARAMETERS_STRUCT(Parameters, (int, i, 0, "integer"),
                         (std::string, name, "output", "output file"));

int main(int argc, char *argv[]) {
  Parameters p;
  parse_params(argc, argv, p);

  if (p.help_requested) {
    print_help(p);
    return -1;
  }

  print_values(p);
  return 0;
}
