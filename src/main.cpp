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

bool verify_action(std::string const& message);

void enable_virtual_terminal_processing();

std::wstring create_highlighted_file_name(
    std::wstring const& file_name,
    SearchStrategy::Matches const& matches);

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
    program.add_argument("-k", "--kill")
        .help("kill process(es) that have the file handle(s) open")
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
    auto const should_close = program.get<bool>("--close");
    auto const should_kill = program.get<bool>("--kill");
    auto const auto_yes = program.get<bool>("--yes");
    auto const use_regex = program.get<bool>("--regex");

    auto const search_strategy =
        [use_regex, search_query]() -> std::unique_ptr<SearchStrategy> {
        if (use_regex) {
            return RegexSearch::create(*search_query);
        } else {
            return PatternSearch::create(*search_query);
        }
    }();

    if (!search_strategy) {
        return 1;
    }

    bool found = false;

    for (auto const& file_handle : file_handles) {
        if (auto const matches = search_strategy->match(file_handle.file_name);
            !matches.empty()) {
            found = true;

            const std::wstring output = std::format(
                L"[-] Process: 0x{:X} ({})\n"
                L"    Handle:  0x{:X}\n"
                L"    File:    {}",
                file_handle.process_id, file_handle.process_name,
                reinterpret_cast<uintptr_t>(file_handle.file_handle),
                create_highlighted_file_name(file_handle.file_name, matches));
            std::wcout << output << "\n";

            if (should_close) {
                if (auto_yes || verify_action("Are you sure you want to close "
                                              "the file handle? (Y/n): ")) {
                    if (close_handle(file_handle)) {
                        std::cout << "[+] Closed handle\n";
                    } else {
                        std::cerr << "[-] Failed to close handle\n";
                    }
                }
            } else if (should_kill) {
                if (auto_yes || verify_action("Are you sure you want to kill "
                                              "the process? (Y/n): ")) {
                    if (kill_process(file_handle)) {
                        std::cout << "[+] Killed process\n";
                    } else {
                        std::cerr << "[-] Failed to kill process\n";
                    }
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

bool verify_action(std::string const& message) {
    std::cout << message;
    auto const c = static_cast<char>(_getch());
    std::cout << c << "\n\n";

    return toupper(c) == 'Y' || c == '\n' || c == '\r';
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

std::wstring create_highlighted_file_name(
    std::wstring const& file_name,
    SearchStrategy::Matches const& matches) {
    std::wstring highlighted_string;
    highlighted_string.reserve(
        file_name.size() + matches.size() * 2);  // 2 for the escape sequences

    std::wstring highlighted_file_name;
    size_t start_pos = 0;
    for (auto const& match : matches) {
        highlighted_file_name +=
            file_name.substr(start_pos, match.start - start_pos);
        highlighted_file_name += L"\033[94m";
        highlighted_file_name +=
            file_name.substr(match.start, match.end - match.start);
        highlighted_file_name += L"\033[0m";
        start_pos = match.end;
    }

    return highlighted_file_name;
}
