#include <conio.h>
#include <argparse/argparse.hpp>
#include <format>
#include <iostream>
#include <memory>

#include "file_Handles.hpp"
#include "nt/nt_functions.hpp"
#include "search_strategies/pattern_search.hpp"
#include "search_strategies/regex_search.hpp"
#include "utils/debug_privileges.hpp"
#include "utils/string_utils.hpp"

bool verify_unlock();

void enable_virtual_terminal_processing();

int main(int argc, char* argv[]) {
    enable_virtual_terminal_processing();

    argparse::ArgumentParser program("File Handle Closer");

    // Non-positional argument with no name
    program.add_argument("query")
        .help("search query (example: \"file.txt\")")
        .required();
    program.add_argument("-c", "--close")
        .help("close file handle(s)")
        .default_value(false)
        .implicit_value(true);
    program.add_argument("-y", "--yes")
        .help("automatically answer yes to all prompts (not recommended)")
        .default_value(false)
        .implicit_value(true);
    program.add_argument("-r", "--regex")
        .help("use regular expressions for search query")
        .default_value(false)
        .implicit_value(true);

    try {
        program.parse_args(argc, argv);
    } catch (std::runtime_error const& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    if (!utils::set_debug_privilege(true)) {
        std::cerr << "Failed to set debug privilege\n";
        return 1;
    }

    if (!nt::resolve_functions()) {
        std::cerr << "Failed to resolve NtQuerySystemInformation\n";
        return 1;
    }

    auto const file_handles = get_file_handles();

    auto const search_query_utf8 = program.get<std::string>("query");
    auto const search_query = utils::utf8_to_utf16(search_query_utf8, false);
    if (!search_query) {
        std::cerr << "Failed to convert search query to UTF-16\n";
        return 1;
    }
    auto const should_unlock = program.get<bool>("--close");
    auto const auto_yes = program.get<bool>("--yes");
    auto const use_regex = program.get<bool>("--regex");

    std::unique_ptr<SearchStrategy> search_strategy;
    if (use_regex) {
        search_strategy = RegexSearch::create(*search_query);
    } else {
        search_strategy = PatternSearch::create(*search_query);
    }
    if (!search_strategy) {
        return 1;
    }

    bool found = false;

    for (auto const& file_handle : file_handles) {
        if (search_strategy->match(file_handle.file_name)) {
            found = true;

            const std::wstring output = std::format(
                L"[-] Process: 0x{:X} ({})\n"
                L"    Handle:  0x{:X}\n"
                L"    File:    \033[1m{}\033[0m",
                file_handle.process_id, file_handle.process_name,
                reinterpret_cast<uintptr_t>(file_handle.file_handle),
                file_handle.file_name);
            std::wcout << output << "\n";

            if (should_unlock && (auto_yes || verify_unlock())) {
                if (close_handle(file_handle)) {
                    std::cout << "[+] Unlocked handle\n";
                } else {
                    std::cerr << "[-] Failed to unlock handle\n";
                }
            }
        }
    }
    if (!found) {
        std::wcout << "No file handles found with query: \"" << *search_query
                   << "\"\n";
    }

    return 0;
}

bool verify_unlock() {
    std::cout << "Are you sure you want to unlock the file handle(s)? (Y/n): ";
    auto const c = static_cast<char>(_getch());
    std::cout << c << "\n\n";

    return toupper(c) == 'Y' || c == '\n';
}

void enable_virtual_terminal_processing() {
    // Get the standard output handle.
    auto const h_out = GetStdHandle(STD_OUTPUT_HANDLE);

    DWORD dw_mode = 0;
    // Get the current output mode.
    GetConsoleMode(h_out, &dw_mode);

    // Enable the virtual terminal processing flag.
    dw_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    // Set the new mode.
    SetConsoleMode(h_out, dw_mode);
}