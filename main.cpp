#include <boost/hana.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/preprocessor.hpp>
#include <iostream>
#include <string>
#include <vector>

template <typename T> struct parameter {
  using value_type = T;
  parameter(T val, std::string help) : val(val), help(help) {}
  operator T() const { return val; }
  parameter &operator=(const T &x) {
    val = x;
    return *this;
  }
  T val;
  const std::string help;
};

#define DEFINE_PARAMETERS_STRUCT(name, ...)                                    \
  DEFINE_PARAMETERS_STRUCT_IMPL(name, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define DEFINE_PARAMETERS_STRUCT_IMPL(name, members_seq)                       \
  struct name {                                                                \
    GENERATE_MEMBERS(members_seq)                                              \
    bool help_requested = false;                                               \
  };                                                                           \
  BOOST_HANA_ADAPT_STRUCT(name, BOOST_PP_SEQ_ENUM(PARAM_NAME_SEQ(members_seq)))

#define PARAM_NAME_SEQ(members_seq)                                            \
  BOOST_PP_SEQ_TRANSFORM(PARAM_NAME_SEQ_OP, , members_seq)

#define PARAM_NAME_SEQ_OP(s, data, member) PARAM_NAME(member)

#define PARAM_TYPE(member) BOOST_PP_TUPLE_ELEM(0, member)
#define PARAM_NAME(member) BOOST_PP_TUPLE_ELEM(1, member)
#define PARAM_DEFAULT(member) BOOST_PP_TUPLE_ELEM(2, member)
#define PARAM_HELP(member) BOOST_PP_TUPLE_ELEM(3, member)

#define GENERATE_MEMBERS(members_seq)                                          \
  BOOST_PP_SEQ_FOR_EACH(GENERATE_MEMBER, , members_seq)

#define GENERATE_MEMBER(r, data, member)                                       \
  parameter<PARAM_TYPE(member)> PARAM_NAME(member) = {PARAM_DEFAULT(member),   \
                                                      PARAM_HELP(member)};

template <class T> void print_help(const T &params) {
  boost::hana::for_each(params, [](auto pair) {
    std::cout << "--" << boost::hana::to<const char *>(boost::hana::first(pair))
              << " arg (=" << boost::hana::second(pair).val << ") "
              << boost::hana::second(pair).help << "\n";
  });
}

template <class T> void print_values(const T &params) {
  boost::hana::for_each(params, [](auto pair) {
    std::cout << boost::hana::to<const char *>(boost::hana::first(pair))
              << " : " << boost::hana::second(pair).val << "\n";
  });
}

template <class T> void parse_params(int argc, char *argv[], T &params) {
  std::vector<std::string> args;
  for (int i = 1; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }
  if (std::find(args.begin(), args.end(), "--help") != args.end()) {
    params.help_requested = true;
    return;
  }

  boost::hana::for_each(boost::hana::accessors<T>(), [&args,
                                                      &params](auto &&pair) {
    auto name = boost::hana::to<const char *>(boost::hana::first(pair));
    auto &member = boost::hana::second(pair)(params);

    std::string arg_str("--");
    arg_str += name;

    for (int i = 0; i < args.size(); ++i) {
      if (args[i] == arg_str) {
        member.val = boost::lexical_cast<
            typename std::decay_t<decltype(member)>::value_type>(args[i + 1]);
      }
    }
  });
}

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
