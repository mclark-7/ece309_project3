// src/tools/workspace.cpp — YOU IMPLEMENT (spec §4.4).
//
// Workspace::resolve: a purely lexical check, steps 1–6 of the spec.
// Do not touch the filesystem here (no exists(), canonical(), ...).
// The constructor, root() and check_symlinks() are provided in
// workspace_provided.cpp.

#include "tools/workspace.h"

namespace fs = std::filesystem;


bool Workspace::resolve(std::string_view user_path,
                        fs::path* out,
                        std::string* reason) const {

    // Check if empty
    if (user_path.empty()) {
        *reason = "empty path";
        return false;
    }

    // Check for Nulls
    if (user_path.find('\0') != std::string_view::npos) {
        *reason = "invalid path";
        return false;
    }
    fs::path path{std::string(user_path)};

    // Reject absolute paths
    if (path.is_absolute() || path.has_root_path()) {
        *reason = "absolute path not allowed";
        return false;
    }
    fs::path normal = path.lexically_normal();

    // Did it leave out workspace
    if (*normal.begin() == "..") {
        *reason = "path outside workspace";
        return false;
    }

    // Create final path
    if (normal == ".") {
        *out = root();
    }
    else {
        *out = root() / normal;
    }
    return true;
}

