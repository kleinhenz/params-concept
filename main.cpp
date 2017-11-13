#include <array>
#include <tuple>
#include <boost/preprocessor.hpp>
#include <iostream>
#include <string>

template <typename T> struct parameter {
  parameter(T val, std::string name, std::string help)
      : val(val), name(name), help(help) {}
  operator T() const { return val; }
  parameter &operator=(const T &x) { val = x; return *this;}
  T val;
  const std::string name;
  const std::string help;
};

#define DEFINE_PARAMETERS_STRUCT(name, ...)                                    \
  DEFINE_PARAMETERS_STRUCT_IMPL(name, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define DEFINE_PARAMETERS_STRUCT_IMPL(name, members_seq)                       \
  struct name {                                                                \
    static constexpr size_t size = BOOST_PP_SEQ_SIZE(members_seq);             \
    GENERATE_MEMBERS(members_seq)                                              \
  };                                                                           \
  template <size_t I> auto &get(name &x);                                      \
  GENERATE_GETS(name, members_seq);

#define PARAM_TYPE_SEQ(members_seq) \
  BOOST_PP_SEQ_TRANSFORM(PARAM_TYPE_SEQ_OP,_,members_seq)

#define PARAM_TYPE_SEQ_OP(s, data, member) \
  PARAM_TYPE(member)

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

#define GENERATE_GETS(name, members_seq)                                       \
  BOOST_PP_SEQ_FOR_EACH_I(GENERATE_GET, name, members_seq)

#define GENERATE_GET(r, name, i, member)                                       \
  template <> auto &get<i>(name & x) { return x.PARAM_NAME(member); }

DEFINE_PARAMETERS_STRUCT(Parameters,
                         (int, i, 0, "integer"),
                         (std::string, name, "output", "output file"))

template<class F>
void for_each_impl(Parameters &p, F&& f, std::integral_constant<size_t, Parameters::size-1>){
  f(get<Parameters::size-1>(p));
}

template<size_t N, class F>
void for_each_impl(Parameters &p, F&& f, std::integral_constant<size_t, N>){
  f(get<N>(p));
  for_each_impl(p, std::forward<F>(f), std::integral_constant<size_t, N+1>{});
}

template<class F>
void for_each(Parameters &p, F&& f) {
  for_each_impl(p, std::forward<F>(f), std::integral_constant<size_t, 0>{});
}

auto print_params = [](auto p){std::cout << p.name << " = " << p.val << "\n";};
auto print_help = [](auto p){std::cout << "  --" << p.name << " arg (=" << p.val << ") " << "\"" << p.help << "\"\n";};

int main(int argc, char *argv[]) {
  Parameters p;
  for_each(p, print_help);
}
