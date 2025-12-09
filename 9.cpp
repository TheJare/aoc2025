#include "base.h"

using namespace std;

struct TPair {
    int i, j;
};

int main(int argc, char** argv) {
    vector<string> lines = split_string_by_newline(read_file_to_string(
        argc > 1 ? argv[1] : filesystem::path{argv[0]}.filename().replace_extension(".txt")));
    lines.erase(remove_if(lines.begin(), lines.end(),
                       [](const string& s) { return s.empty(); }),
                lines.end());

    const int IMG_SIZE = 512;
    ImageStream img(format("output_{:0>2}/frame", filesystem::path(argv[0]).stem().string()),
                    IMG_SIZE, IMG_SIZE);

    long long v1 = 0, v2 = 0;

    vector<TPair> positions;
    TPair pmin{-1,-1}, pmax{0,0};
    for (auto&& line: lines) {
        char c;
        TPair p;
        istringstream il(line);
        il >> p.j >> c >> p.i; // Transposing x & y to make more horizontal-oriented, smaller gif
        positions.push_back(p);
        if (pmin.i < 0 || p.i < pmin.i) pmin.i = p.i;
        if (pmin.j < 0 || p.j < pmin.j) pmin.j = p.j;
        pmax.i = max(pmax.i, p.i);
        pmax.j = max(pmax.j, p.j);
    }

    for (int n = 0; n < positions.size(); ++n) {
        auto && p0 = positions[n];
        p0.i -= pmin.i; // recenter points for convenience
        p0.j -= pmin.j;
        for (int m = 0; m < n; ++m) {
            auto && p1 = positions[m];
            v1 = max(v1, (long long)(abs(p1.i-p0.i)+1)*(abs(p1.j-p0.j)+1));
        }
    }
    int w = pmax.j-pmin.j;
    int h = pmax.i-pmin.i;

    auto draw_circle = [&positions, &img, &w, &h](bool write) {
        for (int n = 0; n < positions.size(); ++n) {
            auto && p0 = positions[n];
            auto && p1 = positions[(n+1) % positions.size()];
            int x0 = min(p0.j, p1.j)*IMG_SIZE/w;
            int y0 = min(p0.i, p1.i)*IMG_SIZE/h;
            int x1 = max(p0.j, p1.j)*IMG_SIZE/w;
            int y1 = max(p0.i, p1.i)*IMG_SIZE/h;
            img.rect(x0, y0, x1+1, y1+1, 255, 255, 255);
            if (write && (n % 20 == 0)) {
                img.write_frame();
            }
        }
    };
    draw_circle(true);

    // part 2
    // Now instead of solving the generic problem for any potentially valid input,
    // we just take advantage of the very visible and verifiable properties of the input:
    // - large seam in the midde means one of the seam's inner corners MUST be a corner
    // - which means that the other corner must be in the same half
    // - the area defined does not have self-intersections or large or adjacent edges
    // - which in turn means that invalid rectangles always have another input point inside them
    // Checks become trivial with this.
    // We could ignore the first (large seam) trick by checking the full edges and not just points,
    // against each rectangle. But since we're already making assumptions, why would we bother not
    // making them all?
    TPair s0, s1;
    for (int n = 0; n < positions.size(); ++n) {
        auto && p0 = positions[n];
        bool imgwritten = false;
        int frame_divider = rand() % (n + 1);
        for (int m = 0; m < n; ++m) {
            auto && p1 = positions[m];

            // I should be discarding points based on half here, but do it later for prettier gif

            // check if any other point is inside. Also extremely simplified condition
            bool full = true;
            int minx = min(p0.j, p1.j);
            int miny = min(p0.i, p1.i);
            int maxx = max(p0.j, p1.j)+1;
            int maxy = max(p0.i, p1.i)+1;
            int mx = min(p0.j, p1.j);
            for (int k = 0; k < positions.size(); ++k) {
                if (k != n && k != m) {
                    auto && pk = positions[k];
                    if (pk.j >= minx && pk.j < maxx && pk.i >= miny && pk.i < maxy) {
                        full = false;
                        break;
                    }
                }
            }
            if (full) {
                img.rect(minx*IMG_SIZE/w, miny*IMG_SIZE/h, maxx*IMG_SIZE/w, maxy*IMG_SIZE/h, rand() % 127 + 127, rand() % 127 + 127, rand() % 127 + 127);
                // Points must be in same half
                if (p0.i < h/2 && p1.i < h/2 || p0.i > h/2 && p1.i > h/2) {
                    if (v2 < (long long)(maxy-miny)*(maxx-minx)) {
                        v2 = (long long)(maxy-miny)*(maxx-minx);
                        s0 = TPair{miny, minx};
                        s1 = TPair{maxy, maxx};
                    }
                }
                if (!imgwritten && m >= frame_divider && (n % 4) == 0) {
                    img.rect(s0.j*IMG_SIZE/w, s0.i*IMG_SIZE/h, s1.j*IMG_SIZE/w, s1.i*IMG_SIZE/h, 254, 254, 254);
                    draw_circle(false);
                    img.fade(3, 255);
                    img.write_frame();
                    imgwritten = true;
                }
            }
        }
    }
    img.rect(s0.j*IMG_SIZE/w, s0.i*IMG_SIZE/h, s1.j*IMG_SIZE/w, s1.i*IMG_SIZE/h, 255, 255, 255);
    draw_circle(false);
    for (int i = 0; i < 100; ++i) {
        img.fade(3, 255);
        img.write_frame();
    }

    cout << format("{}\n{}\n", v1, v2);
    return 0;
}
