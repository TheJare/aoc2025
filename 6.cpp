#include "base.h"

using namespace std;

int main(int argc, char** argv) {
    vector<string> lines = split_string_by_newline(read_file_to_string(
        argc > 1 ? argv[1] : filesystem::path{argv[0]}.filename().replace_extension(".txt")));
    lines.erase(remove_if(lines.begin(), lines.end(),
                       [](const string& s) { return s.empty(); }),
                lines.end());

    ImageStream img(format("output_{:0>2}/frame", filesystem::path(argv[0]).stem().string()),
                    256, 256);

    long long v1 = 0, v2 = 0;

    // Parse the input
    vector<vector<long long>> probleme;
    for (auto it = lines.begin(); it != lines.end(); ) {
        vector<long long> row;
        stringstream ss(*it++);
        while (!ss.eof()) {
            if (it == lines.end()) {
                char op;
                ss >> op >> ws;
                row.push_back(static_cast<long long>(op));
            } else {
                long long v;
                ss >> v >> ws;
                row.push_back(v);
            }
        }
        probleme.push_back(row);
    }
    // p1
    auto && ops = probleme.back();
    for (int i = 0; i < probleme[0].size(); ++i) {
        long long v = ops[i] == '*'? 1 : 0;
        for (int j = 0; j < probleme.size() - 1; ++j) {
            v = ops[i] == '+' ? v + probleme[j][i] : v * probleme[j][i];
        }
        v1 += v;
    }


    // p2 uses the original input but the parsed operators
    int problem_index = 0;
    long long v = ops[problem_index] == '*'? 1 : 0;
    stringbuf ssl;
    for (int i = 0; i < lines[0].size(); ++i) {
        // read column
        for (int j = 0; j < lines.size()-1; ++j) {
            ssl.sputc(lines[j][i]);
        }
        // parse as a num, or as an empty column that triggers end of current problem
        if (img.frame() < 300) {
            img.scroll_up(4);
            img.fade(4);
            img.rect(0, img.height()-4, img.width(), img.height(), 0, 0, 0);
        }
        try {
            long long line_value = stoll(ssl.str());
            if (img.frame() < 300) {
                img.rect(0, img.height()-4, (log10(line_value)+1)*4, img.height(), rand() % 128 + 127,
                         rand() % 128 + 127, rand() % 128 + 127);
            }
            ssl.str(""); // clear for next use and hopefully reuse the internal buffer
            v = ops[problem_index] == '*' ? v * line_value : v + line_value;
        } catch (...) {
            if (img.frame() < 300) {
                img.rect((lines.size()-1)*4, img.height()-4, (lines.size()+log10(v))*4, img.height(), 255, 255, 255);
                img.write_frame();
            }
            v2 += v;
            v = ops[++problem_index] == '*'? 1 : 0;
        }
    }
    v2 += v; // add up the last problem
    for (int i = 0; i < 21; ++i) {
        img.scroll_up(12);
        img.fade(12);
        img.rect(0, img.height()-12, img.width(), img.height(), 0, 0, 0);
        img.write_frame();
    }

    cout << format("{}\n{}\n", v1, v2);
    return 0;
}
