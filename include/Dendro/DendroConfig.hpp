#pragma once
#ifndef _DENDRO_CONFIG_HPP_INCLUDED
#define _DENDRO_CONFIG_HPP_INCLUDED

/// \file DendroConfig.hpp
/// \brief Configuration structure for directory tree generation.

#include <vector>
#include <string>

namespace dendro {

    /// \brief Configuration structure for directory tree generation.
    struct DendroConfig {
        std::vector<std::string> include_dirs;          ///< Directories to include in the tree structure.
        std::vector<std::string> exclude_paths;         ///< Specific directories or files to exclude from the tree.
        std::vector<std::string> allowed_extensions;    ///< List of file extensions to include; if empty, all are allowed.
        std::vector<std::string> excluded_extensions;   ///< List of file extensions to exclude from the output.
        std::string output_file = "structure.txt";      ///< Path to the output file where the structure will be saved.
        std::string root_path;          ///< Root directory for the tree; if empty, the first include directory or current directory is used.
        bool copy_to_clipboard = false; ///< If true, copies the generated structure to the clipboard instead of saving to a file.
        bool use_utf8 = true;           ///< If true, uses UTF-8 characters for tree drawing; otherwise, uses ASCII-compatible symbols.
        bool show_root = false;         ///< If true, includes the root directory name at the top of the tree output.
    };

};

#endif // DENDRO_CONFIG_HPP_INCLUDED
