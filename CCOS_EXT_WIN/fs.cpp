//
// Project: clibparser
// Created by bajdcc
//

#include "pch.h"
#include "fs.h"
#include <ctime>
#include <iterator>
#include <algorithm>
#include <fstream>

#include "../CCGameFramework/base/parser2d/cext.h"

extern clib::cext* global_ext;

namespace clib {

    void vfs_node_dec::advance() {
        if (available())
            idx++;
    }

    int vfs_node_dec::write(byte c) {
        return -1;
    }

    int vfs_node_dec::truncate() {
        return -1;
    }

    void vfs_node_dec::add_handle(int handle, vfs_op_t type)
    {

    }

    vfs_op_t vfs_node_dec::get_handle(int handle, vfs_op_t type)
    {
        return v_none;
    }

    void vfs_node_dec::remove_handle(int handle)
    {

    }

    bool vfs_node_dec::set_data(const std::vector<byte>& data)
    {
        return false;
    }

    bool vfs_node_dec::get_data(std::vector<byte>& data) const
    {
        return false;
    }

    bool vfs_node_dec::set_link(const string_t& data)
    {
        return false;
    }

    bool vfs_node_dec::get_link(string_t& data) const
    {
        return false;
    }

    bool vfs_node_dec::set_time(const std::vector<time_t>& data)
    {
        return false;
    }

    bool vfs_node_dec::get_time(std::vector<time_t>& data) const
    {
        return false;
    }

    int vfs_node_dec::get_length() const
    {
        return -1;
    }

    vfs_node_dec::vfs_node_dec(const vfs_mod_query* mod) : mod(mod) {}

    vfs_node_text::vfs_node_text(const vfs_mod_query* mod, const string_t& str) :
        vfs_node_dec(mod), cache(str) {}

    bool vfs_node_text::available() const {
        return idx < cache.length();
    }

    int vfs_node_text::index() const {
        return idx < cache.length() ? cache[idx] : READ_EOF;
    }

    int vfs_node_text::get_length() const {
        return (int)cache.length();
    }

    bool vfs_node_text::get_data(std::vector<byte>& data) const
    {
        data.resize(cache.size());
        std::copy(cache.begin(), cache.end(), data.begin());
        return true;
    }

    vfs_node_file::vfs_node_file(const vfs_mod_query* mod, const vfs_node::ref& ref) :
        vfs_node_dec(mod), node(ref) {
        node.lock()->refs++;
    }

    vfs_node_file::~vfs_node_file() {
        node.lock()->refs--;
    }

    bool vfs_node_file::available() const {
        auto n = node.lock();
        if (!n)
            return false;
        return idx < n->data.size();
    }

    int vfs_node_file::index() const {
        auto n = node.lock();
        if (!n)
            return READ_ERROR;
        if (idx < n->data.size())
            return n->data[idx];
        return READ_EOF;
    }

    int vfs_node_file::write(byte c) {
        auto n = node.lock();
        if (!n)
            return -1;
        if (!mod->can_mod(n, 1))
            return -2;
        n->data.push_back(c);
        idx = n->data.size() - 1;
        return 0;
    }

    int vfs_node_file::truncate() {
        auto n = node.lock();
        if (!n)
            return -1;
        if (!mod->can_mod(n, 1))
            return -2;
        n->data.clear();
        idx = 0;
        return 0;
    }

    void vfs_node_file::add_handle(int handle, vfs_op_t type)
    {
        assert(this_handle == -1);
        this_handle = handle;
        auto n = node.lock();
        assert(n->handles.find(handle) == n->handles.end());
        n->handles.insert(std::make_pair(handle, type));
        if (type == v_write)
            n->handles_write.push_back(handle);
        else if (type == v_read)
            n->handles_read.push_back(handle);
    }

    vfs_op_t vfs_node_file::get_handle(int handle, vfs_op_t type)
    {
        auto n = node.lock();
        auto f = n->handles.find(handle);
        if (f == n->handles.end())
            return v_none;
        return f->second;
    }

    void vfs_node_file::remove_handle(int handle)
    {
        auto n = node.lock();
        auto f = n->handles.find(handle);
        if (f == n->handles.end())
            return;
        if (f->second == v_write)
            n->handles_write.remove(handle);
        else if (f->second == v_read)
            n->handles_read.remove(handle);
        n->handles.erase(handle);
    }

    bool vfs_node_file::set_data(const std::vector<byte>& data)
    {
        auto n = node.lock();
        if (!n)
            return false;
        if (!mod->can_mod(n, 1))
            return false;
        n->data = data;
        return true;
    }

    bool vfs_node_file::get_data(std::vector<byte>& data) const
    {
        auto n = node.lock();
        if (!n)
            return false;
        if (!mod->can_mod(n, 0))
            return false;
        data = n->data;
        return true;
    }

    bool vfs_node_file::set_time(const std::vector<time_t>& data)
    {
        auto n = node.lock();
        n->time.create = data[0];
        n->time.access = data[1];
        n->time.modify = data[2];
        return true;
    }

    bool vfs_node_file::get_time(std::vector<time_t>& data) const
    {
        auto n = node.lock();
        data[0] = n->time.create;
        data[1] = n->time.access;
        data[2] = n->time.modify;
        return true;
    }

    int vfs_node_file::get_length() const
    {
        auto n = node.lock();
        if (!n)
            return -1;
        if (!mod->can_mod(n, 0))
            return -1;
        return (int)n->data.size();
    }

    // ---------------------------------------------

    cextfs::cextfs() {
        reset();
    }

    static void mod_copy(vfs_mod* mod, const char* s) {
        memcpy((char*)mod, s, 9);
    }

    void cextfs::reset() {
        account.clear();
        account.insert({ 0, vfs_user{ 0, "root", "root" } });
        account.insert({ user, vfs_user{ user, "user", "user" } });
        account.insert({ ext, vfs_user{ ext, "ext", "ext" } });
        current_user = 0;
        last_user = 1;
        root = new_node(fs_dir);
        mod_copy(root->mod, "rw-r--rw-"); // make '/' writable
        pwd = "/";
        auto n = now();
        tm t;
        localtime_s(&t, &n);
        year = t.tm_year;
        current_user = 1;
        last_user = 0;
    }

    void cextfs::error(const string_t& str) {
        global_ext->ext_error(str);
    }

    vfs_node::ref cextfs::new_node(vfs_file_t type) {
        auto node = std::make_shared<vfs_node>();
        node->type = type;
        if (type == fs_file) {
            mod_copy(node->mod, "rw-r--r--");
        }
        else if (type == fs_dir) {
            mod_copy(node->mod, "rw-r--r--");
        }
        else {
            error("invalid mod");
        }
        time_t ctime;
        time(&ctime);
        node->time.create = ctime;
        node->time.access = ctime;
        node->time.modify = ctime;
        node->owner = current_user;
        node->refs = 0;
        node->locked = false;
        node->callback = nullptr;
        node->magic = fss_none;
        return node;
    }

    string_t cextfs::get_user() const {
        return account.at(current_user).name;
    }

    string_t cextfs::get_pwd() const {
        return pwd;
    }

    char* cextfs::file_time(const time_t& t) const {
        tm timeptr;
        localtime_s(&timeptr, &t);
        /*static const char wday_name[][4] = {
                "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
        };*/
        static const char mon_name[][4] = {
            "Jan", "Feb", "Mar", "Apr", "May", "Jun",
            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
        };
        static char result[32];
        if (year == timeptr.tm_year) {
            snprintf(result, sizeof(result), "%.3s%3d %.2d:%.2d",
                mon_name[timeptr.tm_mon],
                timeptr.tm_mday, timeptr.tm_hour,
                timeptr.tm_min);
        }
        else {
            snprintf(result, sizeof(result), "%.3s%3d %5d",
                //wday_name[timeptr.tm_wday],
                mon_name[timeptr.tm_mon],
                timeptr.tm_mday,
                1900 + timeptr.tm_year);
        }
        return result;
    }

    extern string_t limit_string(const string_t& s, uint len);

    void cextfs::ll(const string_t& name, const vfs_node::ref& node, std::ostream& os) const {
        if (!node)
            return;
        static const char* types[] = {
            "35EA3F", // file
            "44FC7D", // dir
            "76FC44", // func
            "BCDD29", // magic
            "9AD9FB", // link
        };
        static char fmt[256];
        snprintf(fmt, sizeof(fmt), "\033FFFA0A0A0\033%c%9s \033FFFB3B920\033%4s \033S4\033%9d \033FFF51C2A8\033%s \033FFF%s\033%s\033S4\033",
            node->type == fs_dir ? 'd' : '-',
            (char*)node->mod,
            account.at(node->owner).name.data(),
            node->data.size(),
            file_time(node->time.create),
            types[(int)node->type],
            limit_string(name, 40).data());
        os << fmt << std::endl;
    }

    string_t full_path(const vfs_node::ref& node) {
        std::stringstream ss;
        std::vector<vfs_node::ref> paths;
        auto p = node;
        while (p) {
            paths.push_back(p);
            p = p->parent.lock();
        }
        std::transform(paths.rbegin(), paths.rend(),
            std::ostream_iterator<string_t>(ss, "/"),
            [](const auto& p) { return p->name; });
        auto str = ss.str();
        str.erase(str.begin() + str.size() - 1);
        return std::move(str);
    }

    int cextfs::macro(const std::vector<string_t>& m, const vfs_node::ref& node, vfs_node_dec** dec) const {
        if (m[1] == "ls") {
            std::stringstream ss;
            std::transform(node->children.begin(), node->children.end(),
                std::ostream_iterator<string_t>(ss, "\n"),
                [](const auto& p) { return p.first; });
            auto str = ss.str();
            if (!str.empty())
                str.pop_back();
            *dec = new vfs_node_text(this, str);
            return 0;
        }
        if (m[1] == "ll") {
            std::stringstream ss;
            ll("..", node->parent.lock(), ss); // parent
            ll(".", node, ss); // self
            for (auto& c : node->children) {
                ll(c.first, c.second, ss); // children
            }
            auto str = ss.str();
            if (!str.empty())
                str.pop_back();
            *dec = new vfs_node_text(this, str);
            return 0;
        }
        if (m[1] == "tree") {
            std::stringstream ss;
            std::vector<vfs_node::ref> stacks;
            stacks.push_back(node);
            while (!stacks.empty()) {
                auto n = stacks.back();
                stacks.pop_back();
                if (!can_mod(node, 0))
                    continue;
                ll(full_path(n), n, ss); // children
                if (n->type == fs_dir) {
                    for (auto c = n->children.rbegin(); c != n->children.rend(); c++) {
                        stacks.push_back(c->second);
                    }
                }
            }
            auto str = ss.str();
            if (!str.empty())
                str.pop_back();
            *dec = new vfs_node_text(this, str);
            return 0;
        }
        return -2;
    }

    int cextfs::get(const string_t& path, vfs_node_dec** dec, vfs_func_t* f) const {
        if (path.empty())
            return -1;
        std::vector<string_t> m;
        split_path(path, m, ':');
        auto p = combine(pwd, m[0]);
        auto node = get_node(p);
        if (!node)
            return -1;
        if (node->type == fs_file) {
            if (node->locked)
                return -3;
            node->time.access = now();
            if (dec) {
                if (m.size() > 1) {
                    return macrofile(m, node, dec);
                }
                *dec = new vfs_node_file(this, node);
            }
            return 0;
        }
        else if (node->type == fs_func) {
            node->time.access = now();
            if (dec) {
                if (f) {
                    auto t = f->stream_type(p);
                    if (t == fss_none) {
                        *dec = new vfs_node_text(this, f->stream_callback(p));
                    }
                    else {
                        *dec = f->stream_create(this, t, p);
                    }
                }
                else {
                    return -2;
                }
            }
            return 0;
        }
        else if (node->type == fs_dir) {
            if (m.size() > 1) {
                return macro(m, node, dec);
            }
        }
        else if (node->type == fs_magic) {
            node->time.access = now();
            *dec = node->callback->stream_create(this, node->magic, p);
            if (*dec == nullptr) {
                return -1;
            }
            return 0;
        }
        return -2;
    }

    bool cextfs::exist_vfs(const string_t& path) const
    {
        auto node = get_node(path);
        if (!node)
            return false;
        if (node->type != fs_file)
            return false;
        return true;
    }

    bool cextfs::read_vfs(const string_t& path, std::vector<byte>& data) const {
        auto node = get_node(path);
        if (!node)
            return false;
        if (node->type != fs_file)
            return false;
        data.resize(node->data.size());
        std::copy(node->data.begin(), node->data.end(), data.begin());
        return true;
    }

    void cextfs::as_root(bool flag) {
        if (flag) {
            if (current_user != 0) {
                last_user = current_user;
                current_user = 0;
            }
        }
        else {
            if (current_user == 0) {
                current_user = last_user;
                last_user = 0;
            }
        }
    }

    void cextfs::as_user(int uid, bool flag)
    {
        if (uid == 0)
            return;
        if (flag) {
            last_user = current_user;
            current_user = uid;
        }
        else {
            current_user = last_user;
            last_user = uid;
        }
    }

    bool cextfs::write_vfs(const string_t& path, const std::vector<byte>& data) {
        auto node = get_node(path);
        if (!node) {
            touch(path);
            node = get_node(path);
            if (!node)
                return false;
        }
        if (node->type != fs_file)
            return false;
        if (!node->data.empty())
            return false;
        node->data.resize(data.size());
        std::copy(data.begin(), data.end(), node->data.begin());
        return true;
    }

    bool cextfs::write_text(const string_t& path, const string_t& data)
    {
        return write_vfs(path, std::vector<byte>(data.begin(), data.end()));
    }

    string_t get_parent(const string_t& path) {
        assert(path[0] == '/');
        if (path == "/")
            return path;
        auto f = path.find_last_of('/');
        assert(f != string_t::npos);
        if (f == 0)
            return "/";
        return path.substr(0, f);
    }

    time_t cextfs::now() {
        time_t ctime;
        time(&ctime);
        return ctime;
    }

    void cextfs::split_path(const string_t& path, std::vector<string_t>& args, char c) {
        std::stringstream ss(path);
        string_t temp;
        while (std::getline(ss, temp, c)) {
            args.push_back(temp);
        }
    }

    vfs_node::ref cextfs::get_node(const string_t& path) const {
        std::vector<string_t> paths;
        split_path(path, paths, '/');
        auto cur = root;
        for (size_t i = 0; i < paths.size(); ++i) {
            if (!can_mod(cur, 0))
                return nullptr;
            auto& p = paths[i];
            if (!p.empty()) {
                auto f = cur->children.find(p);
                if (f != cur->children.end()) {
                    if (f->second->type == fs_magic) {
                        return f->second;
                    }
                    if (i < paths.size() - 1 && f->second->type != fs_dir)
                        return nullptr;
                    cur = f->second;
                }
                else {
                    return nullptr;
                }
            }
        }
        return cur;
    }

    int cextfs::_mkdir(const string_t& path, vfs_node::ref& cur) {
        std::vector<string_t> paths;
        split_path(path, paths, '/');
        cur = root;
        bool update = false;
        for (auto& p : paths) {
            if (!p.empty()) {
                auto f = cur->children.find(p);
                if (f != cur->children.end()) {
                    cur = f->second;
                    if (f->second->type != fs_dir)
                        return -2;
                }
                else {
                    if (!update)
                        update = true;
                    if (!can_mod(cur, 1))
                        return -3;
                    auto node = new_node(fs_dir);
                    node->parent = cur;
                    node->name = p;
                    cur->children.insert(std::make_pair(p, node));
                    cur = node;
                }
            }
        }
        if (update)
            return 0;
        return -1;
    }

    int cextfs::mkdir(const string_t& path) {
        auto p = combine(pwd, path);
        vfs_node::ref cur;
        return _mkdir(p, cur);
    }

    string_t cextfs::combine(const string_t& pwd, const string_t& path) const {
        if (path.empty())
            return pwd;
        auto res = (path[0] == '/') ? "/" : pwd;
        std::vector<string_t> paths;
        split_path(path, paths, '/');
        for (auto& p : paths) {
            if (!p.empty()) {
                if (p == ".")
                    continue;
                else if (p == "..")
                    res = get_parent(res);
                else if (res.back() == '/')
                    res += p;
                else
                    res += "/" + p;
            }
        }
        return res;
    }

    int cextfs::touch(const string_t& path) {
        auto p = combine(pwd, path);
        auto node = get_node(p);
        if (!node) {
            vfs_node::ref cur;
            auto s = _mkdir(p, cur);
            if (s == 0) { // new dir
                cur->type = fs_file;
                return -1;
            }
            else { // exists
                _touch(cur);
                return 0;
            }
        }
        if (!can_mod(node, 1))
            return -3;
        switch (node->type) {
        case fs_file:
        case fs_dir:
            _touch(node);
            return 0;
        default:
            return -2;
        }
    }

    void cextfs::_touch(vfs_node::ref& node) {
        auto ctime = now();
        node->time.create = ctime;
        node->time.access = ctime;
        node->time.modify = ctime;
    }

    int cextfs::func(const string_t& path, vfs_func_t* f) {
        auto node = get_node(path);
        if (!node) {
            vfs_node::ref cur;
            auto s = _mkdir(path, cur);
            if (s == 0) { // new dir
                cur->type = fs_func;
                cur->callback = f;
                return 0;
            }
            else { // exists
                return 1;
            }
        }
        return -2;
    }

    int cextfs::magic(const string_t& path, vfs_func_t* f, vfs_stream_t magic) {
        auto node = get_node(path);
        if (!node) {
            vfs_node::ref cur;
            auto s = _mkdir(path, cur);
            if (s == 0) { // new dir
                cur->type = fs_magic;
                cur->magic = magic;
                cur->callback = f;
                return 0;
            }
            else { // exists
                return 1;
            }
        }
        return -2;
    }

    string_t cextfs::get_filename(const string_t& path) {
        if (path.empty())
            return "";
        if (path == "/")
            return "";
        auto f = path.find_last_of('/');
        if (f == string_t::npos)
            return "";
        return path.substr(f + 1);
    }

    static uint64 sum_fs(const std::shared_ptr<vfs_node>& node)
    {
        if (node->type == fs_dir) {
            uint64 s = 0ULL;
            for (const auto& c : node->children) {
                s += sum_fs(c.second);
            }
            return s;
        }
        else if (node->type == fs_file) {
            return node->data.size();
        }
        return 0ULL;
    }

    uint64 cextfs::size() const
    {
        return sum_fs(root);
    }

    int cextfs::rm(const string_t& path) {
        auto p = combine(pwd, path);
        auto node = get_node(p);
        if (!node)
            return -1;
        return node->parent.lock()->children.erase(get_filename(path)) == 0 ?
            -2 : (node->type != fs_dir ? 0 : 1);
    }

    int cextfs::rm_safe(const string_t& path) {
        auto p = combine(pwd, path);
        auto node = get_node(p);
        if (!node)
            return -1;
        if (!can_rm(node))
            return -2;
        return node->parent.lock()->children.erase(get_filename(path)) == 0 ?
            -3 : (node->type != fs_dir ? 0 : 1);
    }

    bool cextfs::can_rm(const vfs_node::ref& node) const {
        if (!can_mod(node, 1))
            return false;
        if (node->refs > 0)
            return false;
        if (node->locked)
            return false;
        if (node->type == fs_dir) {
            for (auto& c : node->children) {
                if (!can_rm(c.second))
                    return false;
            }
        }
        return true;
    }

    bool cextfs::can_mod(const vfs_node::ref& node, int mod) const {
        if (mod != -1) {
            if (node->owner == current_user)
                return node->mod[0].rwx[mod] != '-';
            else {
                if (node->mod[1].rwx[mod] != '-')
                    return true;
                if (node->mod[2].rwx[mod] != '-')
                    return true;
            }
            return false;
        }
        return true;
    }

    int cextfs::macrofile(const std::vector<string_t>& m, const vfs_node::ref& node, vfs_node_dec** dec) const
    {
        static char buf[512];
        if (m[1] == "sys" && m.size() > 2) {
            if (m[2] == "computer_name") {
                DWORD size = 0;
                auto str = GetComputerNameA(nullptr, &size);
                if (size > sizeof(buf)) {
                    auto b = new char[size];
                    if (!GetComputerNameA(b, &size)) {
                        return -4;
                    }
                    *dec = new vfs_node_text(this, b);
                    delete[]b;
                }
                else {
                    if (!GetComputerNameA(buf, &size)) {
                        return -4;
                    }
                    *dec = new vfs_node_text(this, buf);
                }
                return 0;
            }
            else if (m[2] == "user_name") {
                DWORD size = 0;
                auto str = GetUserNameA(nullptr, &size);
                if (size > sizeof(buf)) {
                    auto b = new char[size];
                    if (!GetUserNameA(b, &size)) {
                        return -4;
                    }
                    *dec = new vfs_node_text(this, b);
                    delete[]b;
                }
                else {
                    if (!GetUserNameA(buf, &size)) {
                        return -4;
                    }
                    *dec = new vfs_node_text(this, buf);
                }
                return 0;
            }
        }
        else if (m[1] == "window" && m.size() > 2) {
            if (m[2] == "find_by_name") {
                DWORD size = 0;
                decltype(node->data) v(node->data);
                v.push_back(0);
                auto hwnd = (DWORD)FindWindowA(nullptr, (char*)v.data());
                snprintf(buf, sizeof(buf), "%8X", hwnd);
                auto str = string_t(buf);
                node->data.resize(str.length());
                std::copy(str.begin(), str.end(), node->data.begin());
                *dec = new vfs_node_text(this, str);
                return 0;
            }
            else if (m[2] == "send_message" && m.size() > 3) {
                DWORD size = 0;
                decltype(node->data) v(node->data);
                v.push_back(0);
                DWORD hwnd = 0;
                _snscanf_s((char*)v.data(), v.size() - 1, "%X", &hwnd);
                snprintf(buf, sizeof(buf), "%d", hwnd);
                UINT msg = 0;
                _snscanf_s(m[3].data(), m[3].size(), "%X", &msg);
                DWORD p[2] = { 0,0 };
                auto pi = 0;
                if (m.size() > 4) {
                    _snscanf_s(m[4].data(), m[4].size(), "%X", &p[pi]);
                    pi++;
                }
                if (m.size() > 5) {
                    _snscanf_s(m[5].data(), m[5].size(), "%X", &p[pi]);
                }
                if (hwnd != 0 && msg != 0) {
                    auto ret = SendMessageA((HWND)hwnd, msg, p[0], p[1]);
                    snprintf(buf, sizeof(buf), "HWND= %08X, MSG= %08X, P1= %08X, P2= %08X, RET= %08X", hwnd, msg, p[0], p[1], ret);
                    *dec = new vfs_node_text(this, buf);
                    return 0;
                }
                return -4;
            }
            else if (m[2] == "post_message" && m.size() > 3) {
                DWORD size = 0;
                decltype(node->data) v(node->data);
                v.push_back(0);
                DWORD hwnd = 0;
                _snscanf_s((char*)v.data(), v.size() - 1, "%X", &hwnd);
                snprintf(buf, sizeof(buf), "%d", hwnd);
                UINT msg = 0;
                _snscanf_s(m[3].data(), m[3].size(), "%X", &msg);
                DWORD p[2] = { 0,0 };
                auto pi = 0;
                if (m.size() > 4) {
                    _snscanf_s(m[4].data(), m[4].size(), "%X", &p[pi]);
                    pi++;
                }
                if (m.size() > 5) {
                    _snscanf_s(m[5].data(), m[5].size(), "%X", &p[pi]);
                }
                if (hwnd != 0 && msg != 0) {
                    auto ret = PostMessageA((HWND)hwnd, msg, p[0], p[1]);
                    snprintf(buf, sizeof(buf), "HWND= %08X, MSG= %08X, P1= %08X, P2= %08X, RET= %d", hwnd, msg, p[0], p[1], ret);
                    *dec = new vfs_node_text(this, buf);
                    return 0;
                }
                return -4;
            }
            else if (m[2] == "message_box") {
                DWORD size = 0;
                decltype(node->data) v(node->data);
                v.push_back(0);
                DWORD hwnd = 0;
                _snscanf_s((char*)v.data(), v.size() - 1, "%X", &hwnd);
                snprintf(buf, sizeof(buf), "%d", hwnd);
                string_t cap;
                if (m.size() > 3) {
                    cap = m[3];
                }
                string_t msg;
                if (m.size() > 4) {
                    msg = m[4];
                }
                UINT bt = 0;
                if (m.size() > 5) {
                    _snscanf_s((char*)m[5].data(), m[5].size(), "%X", &bt);
                }
                auto ret = MessageBoxA((HWND)hwnd, msg.data(), cap.data(), bt);
                snprintf(buf, sizeof(buf), "HWND= %08X, CAP= %s, MSG= %s, BT= %08X, RET= %08X", hwnd, cap.data(), msg.data(), bt, ret);
                *dec = new vfs_node_text(this, buf);
                return 0;
            }
        }
        return -4;
    }
}
