#include <boost/hana.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/preprocessor.hpp>
#include <iostream>
#include <string>
#include <vector>

template<class T>
struct parameter_metadata;

template<class T>
struct is_parameter_struct : std::false_type {};

#define DEFINE_PARAMETERS_STRUCT(name, ...) \
  DEFINE_PARAMETERS_STRUCT_IMPL(name, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define DEFINE_PARAMETERS_STRUCT_IMPL(name, members_seq)                   \
  struct name {                                                            \
    GENERATE_MEMBERS(members_seq)                                          \
  };                                                                       \
  BOOST_HANA_ADAPT_STRUCT(name,                                            \
                          BOOST_PP_SEQ_ENUM(PARAM_NAME_SEQ(members_seq))); \
  template <>                                                              \
  struct parameter_metadata<name> {                                        \
    static constexpr auto help() {                                         \
      return boost::hana::make_tuple(                                      \
          BOOST_PP_SEQ_ENUM(PARAM_HELP_SEQ(members_seq)));                 \
    }                                                                      \
  };                                                                       \
  template <>                                                              \
  struct is_parameter_struct<name> : std::true_type {};

#define PARAM_NAME_SEQ(members_seq) \
  BOOST_PP_SEQ_TRANSFORM(PARAM_NAME_SEQ_OP, , members_seq)

#define PARAM_HELP_SEQ(members_seq) \
  BOOST_PP_SEQ_TRANSFORM(PARAM_HELP_SEQ_OP, , members_seq)

#define PARAM_NAME_SEQ_OP(s, data, member) PARAM_NAME(member)
#define PARAM_HELP_SEQ_OP(s, data, member) PARAM_HELP(member)

#define PARAM_TYPE(member) BOOST_PP_TUPLE_ELEM(0, member)
#define PARAM_NAME(member) BOOST_PP_TUPLE_ELEM(1, member)
#define PARAM_DEFAULT(member) BOOST_PP_TUPLE_ELEM(2, member)
#define PARAM_HELP(member) BOOST_PP_TUPLE_ELEM(3, member)

#define GENERATE_MEMBERS(members_seq) \
  BOOST_PP_SEQ_FOR_EACH(GENERATE_MEMBER, , members_seq)

#define GENERATE_MEMBER(r, data, member) \
  PARAM_TYPE(member) PARAM_NAME(member) = PARAM_DEFAULT(member);

template <typename T>
auto make_name_val_help_tuple(T &&p) {
  static_assert(is_parameter_struct<std::decay_t<T>>::value);
  return boost::hana::zip(boost::hana::keys(std::forward<T>(p)),
                          boost::hana::members(std::forward<T>(p)),
                          parameter_metadata<std::decay_t<T>>::help());
}

template <class T>
void parse_params(int argc, char *argv[], T &p) {
  static_assert(is_parameter_struct<T>::value);

  std::vector<std::string> args;
  for (int i = 1; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }

  boost::hana::for_each(boost::hana::accessors<T>(), [&args, &p](auto &&pair) {
    auto name = boost::hana::to<const char *>(boost::hana::first(pair));
    auto &member = boost::hana::second(pair)(p);

    std::string arg_str("--");
    arg_str += name;

    for (int i = 0; i < args.size(); ++i) {
      if (args[i] == arg_str) {
        member = boost::lexical_cast<typename std::decay_t<decltype(member)>>(
            args[i + 1]);
      }
    }
  });
}

template <class T>
void print_values(std::ostream &os, const T &p) {
  static_assert(is_parameter_struct<T>::value);
  boost::hana::for_each(p, [&os](auto pair) {
    os << boost::hana::to<const char *>(boost::hana::first(pair)) << " : "
       << boost::hana::second(pair) << "\n";
  });
}

template <typename T>
void print_desc(std::ostream &os, const T &p) {
  using namespace boost::hana::literals;
  static_assert(is_parameter_struct<T>::value);
  boost::hana::for_each(make_name_val_help_tuple(p), [&os](auto tup) {
    auto k = tup[0_c];
    auto v = tup[1_c];
    auto h = tup[2_c];
    os << "--" << boost::hana::to<const char *>(k) << " arg (=" << v << ") "
       << boost::hana::to<const char *>(h) << "\n";
  });
}
