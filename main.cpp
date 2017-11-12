#include <array>
#include <boost/preprocessor.hpp>
#include <iostream>
#include <string>

template<typename ... Types> struct type_list {};

template <typename T> struct parameter {
  parameter(T val, std::string name, std::string help)
      : val(val), name(name), help(help) {}
  operator T() const { return val; }
  T val;
  std::string name;
  std::string help;
};

#define DEFINE_PARAMETERS_STRUCT(name, ...)                                    \
  DEFINE_PARAMETERS_STRUCT_IMPL(name, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

// TODO should Parameters type be templated on the contained types???
#define DEFINE_PARAMETERS_STRUCT_IMPL(name, members_seq)                       \
  struct name {                                                                \
    GENERATE_MEMBERS(members_seq)                                              \
  }

#define PARAM_TYPE(member) BOOST_PP_TUPLE_ELEM(0, member)
#define PARAM_NAME(member) BOOST_PP_TUPLE_ELEM(1, member)
#define PARAM_DEFAULT(member) BOOST_PP_TUPLE_ELEM(2, member)
#define PARAM_HELP(member) BOOST_PP_TUPLE_ELEM(3, member)

#define PARAM_NAME_STR(member) BOOST_PP_STRINGIZE(PARAM_NAME(member))

#define GENERATE_MEMBERS(members_seq)                                          \
  BOOST_PP_SEQ_FOR_EACH(GENERATE_MEMBER, , members_seq)

#define GENERATE_MEMBER(r, data, member)                                       \
  parameter<PARAM_TYPE(member)> PARAM_NAME(member) = {                         \
      PARAM_DEFAULT(member), PARAM_NAME_STR(member), PARAM_HELP(member)};

/*
#define GENERATE_GETS(members_seq) \
  BOOST_PP_SEQ_FOR_EACH_I(GENERATE_GET, , members_seq)

#define GENERATE_GET(r, data, i, member) \
  template<size_t i> get(
  */


DEFINE_PARAMETERS_STRUCT(Parameters,
                         (int, i, 0, "i param"),
                         (int, j, 1, "j param"));

/*
template<typename T, size_t I> T get(const Parameters& p) {}
template<> int get<0>(const Parameters &p){return p.i;}
template<> int get<1>(const Parameters &p){return p.j;}
*/

int main(int argc, char *argv[]) {
  Parameters p;

  std::cout << "i = " << p.i << "\n";
  std::cout << "j = " << p.j << "\n";
}
