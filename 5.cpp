#include "base.h"

using namespace std;

int main(int argc, char** argv) {
    vector<string> lines = split_string_by_newline(read_file_to_string(
        argc > 1 ? argv[1] : filesystem::path{argv[0]}.filename().replace_extension(".txt")));

    long long v1 = 0, v2 = 0;

    // read both sections
    auto l = lines.begin();
    long long min_v = -1, max_v = -1;
    vector<pair<long long, long long>> ingredient_ranges;
    while (!l->empty()) {
        long long a, b;
        char c;
        stringstream(*l++) >> a >> c >> b;
        if (a > b) { swap(a, b); } // just in case the input has tricks
        if (min_v == -1 || a < min_v) { min_v = a; }
        max_v = max(max_v, b);
        ingredient_ranges.push_back({a, ++b}); // non-inclusive range on the right much more confy
        cout << a << c << b << endl;
    }


    ImageStream img(format("output_{:0>2}/frame", filesystem::path(argv[0]).stem().string()),
                    ingredient_ranges.size()*2, ingredient_ranges.size()*2);

    ++l;
    do {
        long long v = stoll(*l++);
        cout << v << " ";
        for (auto&& [a, b] : ingredient_ranges) {
            if (v >= a && v < b) {
                v1++;
                break;
            }
        }   
    } while (l != lines.end() && !l->empty());

    // bast initial ranges to gray
    for (auto itr = ingredient_ranges.begin(); itr != ingredient_ranges.end(); ++itr) {
        auto [a, b] = *itr;
        if (a == b) { continue; }
        int x0 = (a - min_v)*img.width()/(max_v - min_v);
        int x1 = (b - min_v)*img.width()/(max_v - min_v);
        int y = (itr - ingredient_ranges.begin()) * 2;
        img.rect(x0, y, x1, y+2, 255, 255, 255);
    }
    for (int i = 0; i < 16; ++i) {
        img.write_frame();
        img.fade(8, 128);
    }

    // merge overlapping ranges
    int merges;
    do {
        merges = 0;
        for (auto itr = ingredient_ranges.begin(); itr != ingredient_ranges.end(); ++itr) {
            auto [a, b] = *itr;
            if (a == b) { continue; }

            int x0 = (a - min_v)*img.width()/(max_v - min_v);
            int x1 = (b - min_v)*img.width()/(max_v - min_v);
            int y = (itr - ingredient_ranges.begin()) * 2;
            img.rect(x0, y, x1, y+2, 128, 128, 128);
            for (auto itprev = ingredient_ranges.begin(); itprev != itr; ++itprev) {
                auto [pa, pb] = *itprev;
                if (a > pb || pa > b) { continue; } // ignore no-overlap
                *itprev = {min(a, pa), max(b, pb)}; // make previous range covers this one
                *itr = {0, 0}; // remove this range. Could optimize swapping with last element but meh
                int py = (itprev - ingredient_ranges.begin()) * 2;
                img.rect(x0, py, x1, y+2, 127, 127, 127); // fade out the range that disappears

                // flash the range receiving the merge
                int px0 = (itprev->first - min_v)*img.width()/(max_v - min_v);
                int px1 = (itprev->second - min_v)*img.width()/(max_v - min_v);
                img.rect(px0, py, px1, py+2, 255, 255, 255);

                img.write_frame();
                img.fade(16, 128);

                ++merges;
                break;
            }
        }
    } while (merges > 0);

    // bast all final ranges to white
    for (auto itr = ingredient_ranges.begin(); itr != ingredient_ranges.end(); ++itr) {
        auto [a, b] = *itr;
        if (a == b) { continue; }
        int x0 = (a - min_v)*img.width()/(max_v - min_v);
        int x1 = (b - min_v)*img.width()/(max_v - min_v);
        int y = (itr - ingredient_ranges.begin()) * 2;
        img.rect(x0, y, x1, y+2, 255, 255, 255);
    }

    // push each range up to the top
    for (auto itr = ingredient_ranges.begin(); itr != ingredient_ranges.end(); ++itr) {
        auto [a, b] = *itr;
        if (a == b) { continue; }
        for (auto itprev = ingredient_ranges.begin(); itprev <= itr; ++itprev) {
            auto [a, b] = *itprev;
            if (a == b) { continue; }
            int x0 = (a - min_v)*img.width()/(max_v - min_v);
            int x1 = (b - min_v)*img.width()/(max_v - min_v);
            int y = (itr - ingredient_ranges.begin()) * 2;
            img.rect(x0, 0, x1, y+2, 255, 255, 255);
        }
        img.write_frame();
        img.fade(4, 255);
    }
    // final fadeout
    for (int i = 0; i < 64; ++i) {
        img.write_frame();
        img.fade(4);
    } while (merges > 0);

    // add up total coverage
    for (auto&& [a, b] : ingredient_ranges) {
        v2 += (b - a);
    }

    cout << format("\n{}\n{}\n", v1, v2);
    return 0;
}
