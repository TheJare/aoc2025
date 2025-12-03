#include "base.h"

using namespace std;

long long find_battery(const string& line, int ndigits, int* choices) {
    if (line.length() < ndigits) { return 0; }

    long long v = 0;
    auto cur = line.begin();
    for (int i = ndigits - 1; i >= 0; --i) {
        auto a = max_element(cur, line.end() - i);
        v = v * 10 + *a - '0';
        *choices++ = a - line.begin();
        cur = ++a;
    }
    return v;
}

int main(int argc, char** argv) {
    vector<string> lines = split_string_by_newline(read_file_to_string(
        argc > 1 ? argv[1] : filesystem::path{argv[0]}.filename().replace_extension(".txt")));

    const int IMG_SIZE = 200;
    ImageStream img(format("output_{:0>2}/frame", filesystem::path(argv[0]).stem().string()),
                    IMG_SIZE, IMG_SIZE);

    long long v1 = 0, v2 = 0;

    int choices[12];
    for (auto&& l : lines) {
        v1 += find_battery(l, 2, choices);
        v2 += find_battery(l, 12, choices);
        img.fade(8);
        for (int i = 0; i < 12; ++i) {
            int x = choices[i] * 2;
            int y = (img.frame() * 4) % IMG_SIZE;
            img.rect(x, y, x + 2, y + 4, 128 + (rand() % 128), 128 + (rand() % 128),
                     128 + (rand() % 128));
        }
        img.write_frame();
    }
    for (int i = 0; i < 32; ++i) {
        img.fade(8);
        img.write_frame();
    }
    cout << format("{}\n{}\n", v1, v2);
    return 0;
}
