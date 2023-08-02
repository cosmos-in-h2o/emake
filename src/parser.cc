#include <parser.hh>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string.h>
#include <string>
#include <string_view>

void proxy_t::load_to_cache() { strcpy(cache, token); }
void proxy_t::load_to_var_name() { strcpy(var_name, token); }
void proxy_t::add_target() {
    targets->targets.push_back(target_t{token});
    target = &targets->targets[targets->targets.size() - 1];
    _set_empty(var_name);
}

void proxy_t::add_file() {
    target->files.push_back(var_t<>{token, _visibility(cache)});
    _set_empty(var_name);
}
void proxy_t::invoke_sub_target() { _set_empty(var_name); }
void proxy_t::set_kind() {
    target->kind = _target_kind(token);
    _set_empty(var_name);
}
void proxy_t::add_include_dir() {
    target->include_dirs.push_back(var_t<>{token, _visibility(cache)});
    _set_empty(var_name);
}
void proxy_t::add_definition() {
    target->definition.push_back(str_t{token});
    _set_empty(var_name);
}
void proxy_t::set_bin_dir() {
    target->bin_dir = str_t{token};
    _set_empty(var_name);
}
void proxy_t::set_target_dir() {
    target->target_dir = str_t{token};
    _set_empty(var_name);
}
void proxy_t::add_link_dir() {
    target->link_dirs.push_back(var_t<>{token, _visibility(cache)});
    _set_empty(var_name);
}
void proxy_t::add_link() {
    target->links.push_back(var_t<>{token, _visibility(cache)});
    _set_empty(var_name);
}
void proxy_t::add_dep() {
    auto ptr = _target_ptr(token, *targets);
    // if there is not this target
    if (ptr == nullptr) {
        // case it as a link
        target->links.push_back(var_t<>{token, _visibility(cache)});
        return;
    }
    target->deps.push_back(var_t<target_t *>{ptr, _visibility(cache)});
    _set_empty(var_name);
}
void proxy_t::add_var() {
    if (_is_same(cache, "local")) {
        target->local_vars[str_t{var_name}] = str_t{token};
        _set_empty(cache);
        _set_empty(var_name);

    } else if (_is_same(cache, "global")) {
        targets->vars[str_t{var_name}] = str_t{token};
        _set_empty(cache);
        _set_empty(var_name);
    } else {
        strcpy(cache, "global");
        strcpy(var_name, token);
    }
}
void proxy_t::prepare_var() {
    if (target == nullptr) {
        perror("no target\n");
    } else {
        strcpy(var_name, token);
    }
}
void proxy_t::get_local_var() {
    char *temp = &token[1];
    strcpy(token, target->local_vars[str_t{temp}].data());
}
void proxy_t::get_global_var(char *token, targets_t *targets) {
    char *temp = &token[1];
    strcpy(token, targets->vars[str_t{temp}].data());
}

void modules::read_file(const char *path, str_t &text) {
    std::ifstream file(path, std::ios::in);
    if (!file.is_open()) {
        std::cout << "not fount " << path << std::endl;
        return;
    }
    size_t size = std::filesystem::file_size(path);
    text.resize(size, '\0');
    str_t buffer(1024, '\0');
    while (std::getline(file, buffer)) {
        buffer.push_back('\n');
        if (!_ignore(buffer)) {
            text.append(buffer);
        }
    }
}
void modules::get_var(proxy_t &proxy) {
    // global_var
    if (proxy.token[0] == '@') {
        proxy_t::get_global_var(proxy.token, proxy.targets);
    }
    // local_var
    else if (proxy.token[0] == '&') {
        proxy.get_local_var();
    }
}
void modules::work(proxy_t &proxy) {
    if (_is_modifier(proxy.token)) {
        proxy.load_to_cache();
    } else if (_is_var(proxy.token)) {
        proxy.load_to_var_name();
    } else {
        if (_is_same(proxy.var_name, "target")) {
            proxy.add_target();
        } else if (_is_same(proxy.var_name, "sub_target")) {
            proxy.invoke_sub_target();
        } else if (_is_same(proxy.cache, "local") &&
                   _is_empty(proxy.var_name)) {
            proxy.prepare_var();
        } else if (_is_same(proxy.var_name, "file")) {
            proxy.add_file();
        } else if (_is_same(proxy.var_name, "kind")) {
            proxy.set_kind();
        } else if (_is_same(proxy.var_name, "include_dir")) {
            proxy.add_include_dir();
        } else if (_is_same(proxy.var_name, "definition")) {
            proxy.add_definition();
        } else if (_is_same(proxy.var_name, "bin_dir")) {
            proxy.set_bin_dir();
        } else if (_is_same(proxy.var_name, "target_dir")) {
            proxy.set_target_dir();
        } else if (_is_same(proxy.var_name, "link_dir")) {
            proxy.add_link_dir();
        } else if (_is_same(proxy.var_name, "link")) {
            proxy.add_link();
        } else if (_is_same(proxy.var_name, "dep")) {
            proxy.add_dep();
        } else {
            proxy.add_var();
        }
    }
}

void todo(char *token, targets_t *targets) {
    static char cache[256] = "\0";
    static char var_name[256] = "\0";
    static target_t *target = nullptr;
    proxy_t proxy{token, var_name, targets, target, cache};
    modules::get_var(proxy);
    modules::work(proxy);
}

targets_t *parser(const str_t &path) {
    // read file
    str_t text;
    modules::read_file(path.data(), text);
    // parser
    targets_t *targets = new targets_t;
    modules::split(text, targets, todo);
    return targets;
}
