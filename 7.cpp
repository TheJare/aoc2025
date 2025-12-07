#include "base.h"

using namespace std;

int main(int argc, char** argv) {
    vector<string> lines = split_string_by_newline(read_file_to_string(
        argc > 1 ? argv[1] : filesystem::path{argv[0]}.filename().replace_extension(".txt")));
    lines.erase(remove_if(lines.begin(), lines.end(),
                       [](const string& s) { return s.empty(); }),
                lines.end());

    ImageStream img(format("output_{:0>2}/frame", filesystem::path(argv[0]).stem().string()),
                    lines[0].size()*2, lines.size()*2);

    long long v1 = 0, v2 = 1;

    for (int y = 0; y < lines.size(); ++y) {
        auto&& l = lines[y];
        for (int x = 0; x < l.size(); ++x) {
            if (l[x] == '^') {
                img.rect(x*2, y*2, x*2+2, y*2+2, 192, 192, 192);
            }
        }
    }
    img.write_frame();
    // Palette to display Log10 of heat (# of concurrent rays)
    int palette[3*16] = { // Actual max heat is 13 (10^13) but let's be safe
        0, 0, 255,
        64, 0, 192,
        128, 0, 129,
        192, 0, 64,
        255, 0, 0,
        255, 64, 0,
        255, 128, 0,
        255, 192, 0,
        255, 255, 0,
        255, 255, 64,
        255, 255, 128,
        255, 255, 192,
        255, 255, 255,
        255, 255, 255,
        255, 255, 255,
        255, 255, 255
    };

    auto rays = vector<long long>(lines[0].size(), 0);
    for (int y = 0; y < lines.size(); ++y) {
        auto&& l = lines[y];
        for (int i = 0; i < l.size(); ++i) {
            if (l[i] == 'S') { // Hit ray generator
                rays[i] = 1;
            } else if (l[i] == '^' && rays[i] != 0) { // Ray hits a splitter
                v1++;
                v2 += rays[i];
                rays[i-1] = rays[i-1] + rays[i]; // Duplicate rays to each side
                rays[i+1] = rays[i+1] + rays[i];
                rays[i] = 0;
            }
        }
        int wait_frame = 0;
        for (int x = 0; x < l.size(); ++x) {
            if (rays[x] > 0) {
                int heat = (int)log10(rays[x]) * 3;
                img.rect(x*2, y*2, x*2+2, y*2+2, palette[heat+0], palette[heat+1], palette[heat+2]);
                if (++wait_frame >= y/4) {
                    img.write_frame();
                    wait_frame = 0;
                }
            }
        }
    }
    for (int i = 0; i < 30; ++i) {
        img.write_frame();
    }

    cout << format("{}\n{}\n", v1, v2);
    return 0;
}
