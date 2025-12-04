#include "base.h"

using namespace std;

vector<string> ProcessBoard(const vector<string>& lines, int *removed, ImageStream &img) {
    vector<string> newBoard = lines;
    *removed = 0;
    for (int y = 0; y < lines.size(); ++y) {
        auto& l = lines[y];
        for (int x = 0; x < l.size(); ++x) {
            if (l[x] == '.') { continue; }
            // Check neighbors
            int n = 0;
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if (x+dx < 0 || x+dx >= l.size() || y+dy < 0 || y+dy >= lines.size()) {
                        continue;
                    }
                    n += (lines[y+dy][x+dx] != '.');
                }
            }
            if (n <= 4) {
                newBoard[y][x] = '.';
                img.rect(x*2, y*2, x*2+2, y*2+2, 255, 255, 255);
                (*removed)++;
            }
        }
    }

    // Visualization, fade the picked up nodes while keeping the rest visible
    for (int i = 0; i < 4; ++i) {
        for (int y = 0; y < lines.size(); ++y) {
            auto& l = newBoard[y];
            for (int x = 0; x < l.size(); ++x) {
                if (l[x] != '.') {
                    img.rect(x*2, y*2, x*2+2, y*2+2, 128, 128, 128);
                }
            }
        }
        img.write_frame();
        img.fade(16);
    }

    return newBoard;
}

int main(int argc, char** argv) {
    vector<string> lines = split_string_by_newline(read_file_to_string(
        argc > 1 ? argv[1] : filesystem::path{argv[0]}.filename().replace_extension(".txt")));
    lines.erase(remove_if(lines.begin(), lines.end(),
                       [](const string& s) { return s.empty(); }),
                lines.end());

    ImageStream img(format("output_{:0>2}/frame", filesystem::path(argv[0]).stem().string()),
                    lines[0].size()*2, lines.size()*2);

    int v1 = 0;

    lines = ProcessBoard(lines, &v1, img);
    int v2 = v1;
    int v;
    do {
        lines = ProcessBoard(lines, &v, img);
        v2 += v;
    } while (v > 0);

    cout << format("{}\n{}\n", v1, v2);
    return 0;
}
