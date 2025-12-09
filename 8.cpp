#include "base.h"

using namespace std;

struct TJunction {
    long long x, y, z;
    int group;
};

struct TPair {
    int i, j;
};

int main(int argc, char** argv) {
    vector<string> lines = split_string_by_newline(read_file_to_string(
        argc > 1 ? argv[1] : filesystem::path{argv[0]}.filename().replace_extension(".txt")));
    lines.erase(remove_if(lines.begin(), lines.end(),
                       [](const string& s) { return s.empty(); }),
                lines.end());

    ImageStream img(format("output_{:0>2}/frame", filesystem::path(argv[0]).stem().string()),
                    lines[0].size()*2, lines.size()*2);

    long long v1 = 0, v2 = 1;

    vector<TJunction> junctions;
    for (auto&& line: lines) {
        char c;
        TJunction j;
        istringstream il(line);
        il >> j.x >> c >> j.y >> c >> j.z;
        junctions.push_back(j);
    }

    // Now distances from each to each other: N^2
    vector<vector<long long>> distances; // only need half of it but ok
    vector<int> groups(junctions.size());
    vector<TPair> junction_pairs;

    for (int i = 0; i < junctions.size(); ++i) {
        groups[i] = i;
        vector<long long> dist_line(junctions.size(), 0);
        auto&& j0 = junctions[i];
        for (int j = 0; j < i; ++j) {
            auto&& j1 = junctions[j];
            long long d = (j0.x - j1.x)*(j0.x - j1.x) + (j0.y - j1.y)*(j0.y - j1.y) + (j0.z - j1.z)*(j0.z - j1.z);
            dist_line[j] = d;
            junction_pairs.push_back(TPair{i,j});
        }
        distances.push_back(dist_line);
    }
    ranges::sort(junction_pairs, [&distances](TPair &a, TPair &b) { return distances[a.i][a.j] <= distances[b.i][b.j]; });

    // Group'em up
    vector<int> group_sizes(junctions.size(), 1);
    int total = junctions.size() < 100? 10 : 1000;
    int i = 0;
    while (true) {
        auto &pair = junction_pairs[i++];
        int gi = groups[pair.i], gj = groups[pair.j];
        if (gi == gj) { continue; }

        for (int j = 0; j < groups.size(); ++j) {
            if (groups[j] == gj) {
                groups[j] = gi;
            }
        }
        int ng = group_sizes[gi] + group_sizes[gj];
        group_sizes[gi] = ng;
        group_sizes[gj] = 0;
        if (i == total) {
            auto sorted = vector<int>(group_sizes);
            ranges::sort(sorted, std::greater<int>());
            v1 = sorted[0]*sorted[1]*sorted[2];
        }
        if (ng == junctions.size()) {
            v2 = junctions[pair.i].x*junctions[pair.j].x;
            break;
        }
    }

    cout << format("{}\n{}\n", v1, v2);
    return 0;
}
