#ifndef PARSER_HPP
#define PARSER_HPP
#include <ios>
#include <iostream>
#include <stdio.h>
#include <string>
#include <target.hh>

static bool _is_empty(char ch) {
    if (ch == '\t' || ch == ' ' || ch == '\0' || ch == '\n' || ch == '=')
        return true;
    return false;
}

static bool _is_empty(char *str) {
    if (str != nullptr && str[0] == '\0')
        return true;
    return false;
}

static void _set_empty(char *str) { str[0] = '\0'; }

static bool _is_same(const char *str1, const char *str2) {
    return (strcmp(str1, str2) == 0);
}

static void _add_var(char *cache, targets_t &targets) {}

static bool _is_modifier(char *token) {
    if (_is_same(token, "public") || _is_same(token, "private") ||
        _is_same(token, "local")) {
        return true;
    }
    return false;
}

static bool _is_var(char *token) {
    if (_is_same(token, "target") || _is_same(token, "sub_target") ||
        _is_same(token, "compiler") || _is_same(token, "standard") ||
        _is_same(token, "kind") || _is_same(token, "include_dir") ||
        _is_same(token, "definition") || _is_same(token, "bin_dir") ||
        _is_same(token, "target_dir") || _is_same(token, "link_dir") ||
        _is_same(token, "link") || _is_same(token, "dep")) {
        return true;
    }
    return false;
}

static visibility _visibility(char *str) {
    if (_is_same(str, "public")) {
        _set_empty(str);
        return visibility::PUBLIC;
    } else if (_is_same(str, "private")) {
        _set_empty(str);
        return visibility::PRIVATE;
    }
    return visibility::PRIVATE;
}

static target_kind _target_kind(char *str) {
    if (_is_same(str, "binary"))
        return target_kind::BINARY;
    else if (_is_same(str, "shared"))
        return target_kind::SHARED;
    else if (_is_same(str, "static"))
        return target_kind::STATIC;
    return target_kind::BINARY;
}

static target_t *_target_ptr(char *str, targets_t &targets) {
    for (target_t &target : targets.targets) {
        if (_is_same(str, target.name.c_str())) {
            return &target;
        }
    }
    return nullptr;
}

static bool _ignore(std::string_view str) {
    for (char ch : str) {
        if (_is_empty(ch)) {
            continue;
        } else if (ch == '#') {
            return true;
        } else {
            return false;
        }
    }
    return true;
}
struct proxy_t {
    char *token;
    char *var_name;
    targets_t *targets;
    target_t *&target;
    char *cache;

    void load_to_cache();
    void load_to_var_name();
    void add_target();
    void add_file();
    void invoke_sub_target();
    void set_kind();
    void add_include_dir();
    void add_definition();
    void set_bin_dir();
    void set_target_dir();
    void add_link_dir();
    void add_link();
    void add_dep();
    void add_var();
    void prepare_var();
    void get_local_var();
    static void get_global_var(char *token, targets_t *targets);
};

struct modules {
    static void read_file(const char *path, str_t &text);
    static void get_var(proxy_t &proxy);
    static void work(proxy_t &proxy);
    template <typename TodoCallAble>
    static void split(std::string_view text, targets_t *targets,
                      TodoCallAble todo) {
        char buffer_ch = '\0';
        char buffer_str[256];
        for (int i = 0; i < text.size(); i++) {
            buffer_ch = text[i];
            if (!_is_empty(buffer_ch)) {
                int temp_index = 0;
                while (!_is_empty(text[i])) {
                    buffer_str[temp_index] = text[i];
                    temp_index++;
                    i++;
                }

                buffer_str[temp_index] = '\0';
                todo(buffer_str, targets);
            }
        }
    }
};
void todo(char *token, targets_t *targets);
targets_t *parser(const str_t &path = "emake.build");

#endif
