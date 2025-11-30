#include "base.h"

using namespace std;

int main(int argc, char** argv) {
    vector<string> lines = split_string_by_newline(read_file_to_string(
        argc > 1 ? argv[1] : filesystem::path{argv[0]}.filename().replace_extension(".txt")));
    ImageStream img(format("output_{:0>2}/frame", filesystem::path(argv[0]).stem().string()), 128,
                    128);
    for (auto&& i : lines) {
        img.rect(img.rand_x(), img.rand_y(), img.rand_x(), img.rand_y(), rand() & 255, rand() & 255,
                 rand() & 255);
        img.line(img.rand_x(), img.rand_y(), img.rand_x(), img.rand_y(), rand() & 255, rand() & 255,
                 rand() & 255);
        img.write_frame();
    }
    return 0;
}
