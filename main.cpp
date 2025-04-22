#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <regex>

#ifdef _WIN32
    constexpr auto pipeopen = _popen; 
    constexpr auto pipeclose = _pclose;
#else
    constexpr auto pipeopen = popen;
    constexpr auto pipeclose = pclose;
#endif


std::string runCommand(std::string_view command) {
    // Create a pipe to the command output
    FILE* pipe = pipeopen(command.data(), "r");
    if (!pipe) {
        std::cerr << "Failed to run command: " << command << std::endl;
        exit(1);
    }

    char buffer[128];
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe)) {
        result += buffer;
    }

    pipeclose(pipe);

    return result;
}

void parseSegments(const std::string& output) {
#ifdef _MSC_VER
    std::regex section_regex(R"(SECTION HEADER #\S+\s+((\S+) name\s+|\/\S+ name\s+\((\S+)\)\s+)(\S+) virtual size\s+\S+ virtual address \((\S+) to \S+\))");//(\S+)\s+\S+\s+0x([0-9a-fA-F]+)\s+0x([0-9a-fA-F]+);
#else
    std::regex section_regex(R"(\d+\s(\.\S+)\s+(\S+)\s+(\S+)\s+\S+)");
#endif
    std::smatch match;
    std::string line;

    for (auto it = std::sregex_iterator(output.begin(), output.end(), section_regex); it != std::sregex_iterator(); ++it) 
    {
        match = *it;
#ifdef _MSC_VER
        std::string section_name = std::string(match[2]) + std::string(match[3]);
        size_t size = std::stoi(match[4], nullptr, 16);
        std::string address = match[5];
#else
        std::string section_name = match[1];
        size_t size = std::stoi(match[2], nullptr, 16);
        std::string address = match[3];
#endif
        std::cout << std::format("Section: {:<15}, Address: 0x{}, Size: {:>5}B\n",
            section_name, address, size);
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <executable-path>" << std::endl;
        return 1;
    }

    std::cout << "CTEST_FULL_OUTPUT\n";

    std::string executable_path = argv[1];
#ifdef _MSC_VER
    std::string command = "dumpbin /headers " + executable_path;
#else
    std::string command = "objdump -h " + executable_path;
#endif

    // Run llvm-readobj to get the section details
    std::cout << "Running command: " << command << std::endl;
    std::string output = runCommand(command);

    // Parse and display sections information
    parseSegments(output);

    return 0;
}