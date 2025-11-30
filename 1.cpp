#include "base.h"

using namespace std;

int main(int argc, char** argv) {
    vector<string> lines = split_string_by_newline(read_file_to_string(
        argc > 1 ? argv[1] : filesystem::path{argv[0]}.filename().replace_extension(".txt")));
    for (auto&& i : lines) {
        cout << format(">>> {} <<<\n", i);
    }
    return 0;
}
