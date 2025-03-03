#define APP_VERSION          u8"1.0.0"
#define CONSOLIX_USE_LOGIT   0
#define CONSOLIX_USE_CXXOPTS 1
#define CONSOLIX_USE_JSON    0
#include <consolix/consolix.hpp>
#include <Dendro/Dendro.hpp>

/// \brief Entry point of the program.
int main(int argc, char* argv[]) {
    consolix::add<consolix::TitleComponent>(u8"Dendro " APP_VERSION);
    consolix::add<consolix::CliComponent>("Dendro", "Generates a directory structure tree.", [](consolix::CliOptions& options) {
        options.add_options()
        ("i,include", "Directories to include", cxxopts::value<std::vector<std::string>>())
        ("e,exclude", "Directories or files to exclude", cxxopts::value<std::vector<std::string>>())
        ("o,output", "Output file path", cxxopts::value<std::string>())
        ("c,clipboard", "Copy result to clipboard", cxxopts::value<bool>()->default_value("false"))
        ("allow-ext", "File extensions to include (comma-separated)", cxxopts::value<std::vector<std::string>>())
        ("exclude-ext", "File extensions to exclude (comma-separated)", cxxopts::value<std::vector<std::string>>())
        ("root-path", "Specify the root directory for structure generation", cxxopts::value<std::string>())
        ("show-root", "Display the root directory in the output", cxxopts::value<bool>()->default_value("false"))
        ("h,help", "Show help message");
        options.allow_unrecognised_options();
    });

    consolix::run([](){
        auto args = consolix::get_service<consolix::CliArguments>();
        if (args.count("help")) {
            CONSOLIX_STREAM() << consolix::get_service<consolix::CliOptions>().help();
            consolix::stop();
            return;
        }

        dendro::DendroConfig config = dendro::parse_arguments();

        std::string structure_utf8 = dendro::generate_structure(config);
        config.use_utf8 = false;
        std::string structure_oem437 = dendro::generate_structure(config);

        CONSOLIX_LOGO_STREAM() << consolix::color(consolix::TextColor::DarkCyan) << structure_oem437;

        if (config.copy_to_clipboard) {
            consolix::copy_to_clipboard(structure_utf8);
        } else {
            std::ofstream output_file(config.output_file);
            if (output_file.is_open()) {
                output_file << structure_utf8;
                output_file.close();
            }
        }
        consolix::stop();
    });

    return 0;
}
