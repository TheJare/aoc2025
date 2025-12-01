#include "base.h"

using namespace std;

int main(int argc, char** argv) {
    vector<string> lines = split_string_by_newline(read_file_to_string(
        argc > 1 ? argv[1] : filesystem::path{argv[0]}.filename().replace_extension(".txt")));

    const int IMG_RADIUS = 64;
    ImageStream img(format("output_{:0>2}/frame", filesystem::path(argv[0]).stem().string()),
                    IMG_RADIUS * 2, IMG_RADIUS * 2);

    int pos = 50;
    int v1 = 0, v2 = 0;

    for (auto&& l : lines) {
        if (l.length() < 2) { continue; }
        int dir = l[0] == 'R' ? 1 : -1;
        int val = stoi(l.substr(1));
        int rot = dir * val;
        int newpos = pos + rot;
        int adjustednewpos = ((newpos % 100) + 100) % 100;
        // Magic formula for counting part 2
        int cycles = (newpos < 0) ? (newpos / -100 + (pos != 0))
                                  : (((newpos - 1) / 100) + (adjustednewpos == 0));
        // Visualization
        if (img.frame() < 900) {
            int p = 0;
            img.clear();
            int np;
            do {
                np = (pos + dir * p + 100) % 100;
                bool hit = np == 0 || (p == 0 && pos == 0);
                bool draw = hit || (p % 3 == 0);
                if (draw) {
                    int color = hit ? 255 : 0;
                    img.fade(10);
                    img.line(
                        IMG_RADIUS, IMG_RADIUS,
                        IMG_RADIUS + (int)((IMG_RADIUS - 1) * sin(np * 2 * 3.14159265 / 100.0)),
                        IMG_RADIUS - (int)((IMG_RADIUS - 1) * cos(np * 2 * 3.14159265 / 100.0)),
                        color, 255, color);
                    img.write_frame();
                }
                p++;
            } while (np != adjustednewpos);
        }
        // Final computation
        pos = adjustednewpos;
        v1 += (pos == 0);
        v2 += cycles;
    }
    cout << format("{}\n{}\n", v1, v2);
    return 0;
}
