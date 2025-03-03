#pragma once
#ifndef _DENDRO_UTILS_HPP_INCLUDED
#define _DENDRO_UTILS_HPP_INCLUDED

/// \file DendroUtils.hpp
/// \brief Utility functions for directory tree generation and processing.

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include "DendroConfig.hpp"

namespace dendro {
    namespace fs = std::filesystem;

    /// \brief Recursively writes the directory structure.
    /// \param os Output stream to write the directory structure.
    /// \param path Current directory path.
    /// \param root_path The root directory being processed (used to strip from output).
    /// \param config Configuration structure with include/exclude settings.
    /// \param indent Current indentation level.
    /// \param is_last Indicates if this is the last item at the current level.
    /// \param is_root Indicates if this is the initial root directory.
    void traverse_directory_tree_impl(
            std::ostringstream& os,
            const fs::path& path,
            const fs::path& root_path,
            const DendroConfig& config,
            const std::string& indent,
            bool is_last,
            bool is_root = false) {
        std::error_code ec;

        // Check if the path is in the exclude list (directories or files)
        std::string normalized_path = fs::weakly_canonical(path, ec).string();
        auto is_excluded = std::any_of(config.exclude_paths.begin(), config.exclude_paths.end(),
            [&normalized_path](const std::string& excluded) {
                return fs::weakly_canonical(fs::path(excluded)).string() == normalized_path;
            });
        if (is_excluded) {
            return;
        }

        // Check if the directory exists and is accessible
        if (!fs::exists(path, ec) || !fs::is_directory(path, ec)) {
            return;
        }

        // Define tree drawing symbols based on configuration (UTF-8 or ASCII)
        const std::string corner_up = config.use_utf8 ? u8"└── " : std::string(1, (char)0xC0) + " ";
        const std::string corner_dn = config.use_utf8 ? u8"┌── " : std::string(1, (char)0xDA) + " ";
        const std::string branch    = config.use_utf8 ? u8"├── " : std::string(1, (char)0xC3) + " ";
        const std::string vertical  = config.use_utf8 ? u8"│   " : std::string(1, (char)0xB3) + "   ";

        // Display the directory name, omitting the root if specified
        if (is_root) {
            if (config.show_root || !config.include_dirs.empty()) {
                os << path.filename().string() << "/\n";
            }
        } else {
            // Compute relative path without root directory
            std::string relative_path = fs::relative(path, root_path, ec).string();
            if (!relative_path.empty()) {
                os << indent << (is_last ? corner_up : (os.tellp() == 0 ? corner_dn : branch)) << path.filename().string() << "/\n";
            }
        }

        std::vector<fs::path> elements;
        fs::directory_iterator dir_iter(path, fs::directory_options::skip_permission_denied, ec);
        if (ec) {
            return; // Skip directory if content cannot be retrieved
        }

        for (const auto& entry : dir_iter) {
            const auto& entry_path = entry.path();

            // Normalize path and check exclusion list again for files and folders
            std::string entry_normalized = fs::weakly_canonical(entry_path, ec).string();
            if (std::any_of(config.exclude_paths.begin(), config.exclude_paths.end(),
                [&entry_normalized](const std::string& excluded) {
                    return fs::weakly_canonical(fs::path(excluded)).string() == entry_normalized;
                })) {
                continue;
            }

            const std::string extension = entry_path.extension().string();

            // Skip files with excluded extensions
            if (!config.excluded_extensions.empty() && std::find(config.excluded_extensions.begin(), config.excluded_extensions.end(), extension) != config.excluded_extensions.end()) {
                continue;
            }

            // Skip files if the allowed extension list is not empty and the extension is not present
            if (!config.allowed_extensions.empty() && std::find(config.allowed_extensions.begin(), config.allowed_extensions.end(), extension) == config.allowed_extensions.end()) {
                continue;
            }

            elements.push_back(entry_path);
        }

        std::string next_indent = indent + (is_last ? u8"    " : vertical);
        for (size_t i = 0; i < elements.size(); ++i) {
            bool last = (i == (elements.size() - 1));
            if (fs::is_directory(elements[i])) {
                traverse_directory_tree_impl(os, elements[i], root_path, config, (is_root ? "" : next_indent), last);
            } else {
                os << (is_root ? "" : next_indent) << (last ? corner_up : (os.tellp() == 0 ? corner_dn : branch)) << elements[i].filename().string() << "\n";
            }
        }
    }

    /// \brief Recursively generates the directory structure.
    /// \param config Configuration structure with include/exclude settings.
    /// \return Formatted directory tree as a string.
    std::string generate_structure(const DendroConfig& config) {
        std::ostringstream result;
        fs::path root_path;

        if (!config.include_dirs.empty()) {
            for (const auto& dir : config.include_dirs) {
                root_path = config.root_path.empty() ? fs::current_path() : fs::path(dir);
                traverse_directory_tree_impl(result, fs::path(dir), root_path, config, "", false, true);
            }
        } else {
            root_path = config.root_path.empty() ? fs::current_path() : fs::path(config.root_path);
            traverse_directory_tree_impl(result, root_path, root_path, config, "", false, true);
        }

        return result.str();
    }

#   ifdef CONSOLIX_USE_CXXOPTS

    /// \brief Checks whether the root path is contained within include_dirs or exclude_paths.
    /// \param config Configuration structure containing include and exclude paths.
    /// \return True if root_path is found within the specified paths, otherwise false.
    bool validate_root_path(const DendroConfig& config) {
        std::error_code ec;
        std::string normalized_root = fs::weakly_canonical(config.root_path, ec).string();

        auto contains_root = [&normalized_root, &ec](const std::string& path) {
            return fs::weakly_canonical(fs::path(path), ec).string() == normalized_root;
        };

        return std::any_of(config.include_dirs.begin(), config.include_dirs.end(), contains_root) ||
               std::any_of(config.exclude_paths.begin(), config.exclude_paths.end(), contains_root);
    }

    /// \brief Parses command-line arguments and initializes the configuration.
    /// \return DendroConfig structure initialized with CLI parameters.
    DendroConfig parse_arguments() {
        auto args = consolix::get_service<consolix::CliArguments>();
        DendroConfig config;

        if (args.count("include")) {
            config.include_dirs = args["include"].as<std::vector<std::string>>();
        }

        if (args.count("exclude")) {
            config.exclude_paths = args["exclude"].as<std::vector<std::string>>();
        }

        if (args.count("output")) {
            config.output_file = args["output"].as<std::string>();
        }

        if (args.count("clipboard")) {
            config.copy_to_clipboard = true;
        }

        if (args.count("allow-ext")) {
            config.allowed_extensions = args["allow-ext"].as<std::vector<std::string>>();
        }

        if (args.count("exclude-ext")) {
            config.excluded_extensions = args["exclude-ext"].as<std::vector<std::string>>();
        }

        if (args.count("root-path")) {
            config.root_path = args["root-path"].as<std::string>();
        }

        if (args.count("show-root")) {
            config.show_root = args["show-root"].as<bool>();
        }

        // Validate root_path to ensure it is included in the directory structure
        if (!config.root_path.empty() && !validate_root_path(config)) {
            throw std::runtime_error("The specified root_path is not found in the included or excluded paths.");
        }

        return config;
    }
#   endif

};

#endif // _DENDRO_UTILS_HPP_INCLUDED
