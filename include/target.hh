#ifndef TARGET_HH
#define TARGET_HH
#include <string>
#include <vector>
#include <unordered_map>
enum class target_kind { BINARY, SHARED, STATIC };
enum class visibility { PUBLIC, PRIVATE };
using str_t = std::string;
template <typename T_ = str_t> struct var_t {
    T_ var;
    visibility vis = visibility::PRIVATE;
};
template <typename T_ = var_t<>> using arr_t = std::vector<T_>;

struct target_t {
    str_t name;
	arr_t<> files;
    target_kind kind = target_kind::BINARY;
    str_t bin_dir = "build/bin";
    str_t target_dir = "build/bin";
    arr_t<> include_dirs;
    arr_t<str_t> definition;
    arr_t<> link_dirs;
    arr_t<> links;
    arr_t<var_t<target_t *>> deps;
    arr_t<> flags;
    std::unordered_map<str_t, str_t> local_vars;
};

struct targets_t {
    std::vector<target_t> targets;
    str_t compiler = "clang";
    str_t standard = "c++20";
    std::unordered_map<str_t, str_t> vars;
};
#endif
