#include "base.h"

using namespace std;

int main(int argc, char** argv) {
    string input = read_file_to_string(
        argc > 1 ? argv[1] : filesystem::path{argv[0]}.filename().replace_extension(".txt"));

    regex re("[\\,\\-]");
    sregex_token_iterator first{input.begin(), input.end(), re, -1}, last;
    vector<long long> numbers;
    transform(first, last, back_inserter(numbers), [](const string& s) { return stoll(s); });

    const int IMG_RADIUS = 128;
    ImageStream img(format("output_{:0>2}/frame", filesystem::path(argv[0]).stem().string()),
                    IMG_RADIUS * 2, IMG_RADIUS * 2);

    long long v1 = 0, v2 = 0;

    for (size_t i = 0; i < numbers.size(); i += 2) {
        int num_in_range = 0;
        long long a = numbers[i], b = numbers[i + 1];
        // Scan the range, bruteforce style
        while (a <= b) {
            // Find number of repeated digit groups with most digits
            int ndigits = log10(a) + 1;
            int minrepeats = -1; // For resolving the problem - min number of (larger) groups
            int maxrepeats = -1; // For the visualization - max number of (smaller) groups
            // Check all possible group sizes
            for (size_t len = 1; len <= ndigits / 2; ++len) {
                if (ndigits % len != 0) { continue; }
                int groups = ndigits / len;
                long long base = a % (long long)pow(10, len);
                int g = 1;
                // check all groups to see if they match the base
                for (; g < groups; ++g) {
                    if ((a / (long long)pow(10, g * len)) % (long long)pow(10, len) != base) {
                        break;
                    }
                }
                if (g == groups) {
                    minrepeats = groups;
                    if (maxrepeats == -1) { maxrepeats = groups; }
                }
            }
            if (minrepeats == 2) { v1 += a; }
            if (minrepeats > 1) {
                v2 += a;
                num_in_range++;

                // Visualization - max 30 frames per range to avoid being boring
                if (num_in_range < 30 && img.frame() < 900) {
                    const int DIGIT_W = IMG_RADIUS / 4;
                    const int DIGIT_H = IMG_RADIUS / 16;
                    const int GROUP_SPACING = IMG_RADIUS / 16;
                    img.fade(10);
                    int y = (img.frame() % (IMG_RADIUS * 2 / DIGIT_H)) * DIGIT_H;
                    int len = ndigits / maxrepeats;
                    int w = ndigits * DIGIT_W + (maxrepeats - 1) * GROUP_SPACING;

                    // Draw the groups of digits with random colors and a black separator
                    for (int x = (img.width() - w) / 2, n = 0; n < maxrepeats; ++n) {
                        int x1 = x + len * DIGIT_W;
                        img.rect(x, y, x1, y + DIGIT_H, rand() % 128 + 128,
                                 rand() % 128 + 128, rand() % 128 + 128);
                        img.rect(x1, y, x1 + GROUP_SPACING, y + DIGIT_H, 0, 0, 0);
                        x = x1 + 8;
                    }
                    img.write_frame();
                }
            }
            a++;
        }
    }
    cout << format("{}\n{}\n", v1, v2);
    return 0;
}
