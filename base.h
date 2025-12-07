#pragma once

#ifndef AOC2025_BASE_H
#define AOC2025_BASE_H

#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>
#include <stdio.h>
#include <string>
#include <string_view>
#include <vector>
#include <regex>

static std::vector<std::string> split_string_by_newline(const std::string_view str) {
    // This is not compatible with clang 21 on Linux, but it works on clang 21 Windows?
    // Must be some shenanigans with the MSVC standard library implementation making it work.
    // return str | std::ranges::views::split('\n') |
    //        std::ranges::to<std::vector<std::string>>();

    std::vector<std::string> result;
    std::ranges::transform(
        str | std::ranges::views::split('\n'), std::back_inserter(result),
        [](auto&& inner_range) { return std::string(inner_range.begin(), inner_range.end()); });
    return result;
}

static std::string read_file_to_string(const std::filesystem::path& path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

class ImageStream {
public:
    ImageStream(std::string_view filename, int width, int height)
        : m_width(width), m_height(height), m_frame(0), m_filename_path{filename} {
        m_data.resize(width * height * 3);
        std::filesystem::create_directories(m_filename_path.parent_path());
    }

    void set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
        if (x < 0 || x >= m_width || y < 0 || y >= m_height) { return; }
        int index = (y * m_width + x) * 3;
        m_data[index + 0] = r;
        m_data[index + 1] = g;
        m_data[index + 2] = b;
    }

    void write_frame() {
        std::ofstream frame_file(std::format("{}{:04d}.ppm", m_filename_path.string(), m_frame),
                                 std::ios::binary);
        std::cout << std::format("{}{:04d}.ppm", m_filename_path.string(), m_frame) << std::endl;
        frame_file << std::format("P6\n{} {}\n255\n", m_width, m_height);
        frame_file.write(reinterpret_cast<const char*>(m_data.data()), m_data.size());
        m_frame++;
    }

    void clear() { std::fill(m_data.begin(), m_data.end(), 0); }

    void fade(int n) {
        for (auto&& v: m_data) {
            v = static_cast<uint8_t>(std::max(0, v - n));
        }
    }
    void fade(int n, int targetv) {
        for (auto&& v: m_data) {
            if (v >= targetv) {
                v = static_cast<uint8_t>(std::max(targetv, v - n));
            } else {
                v = static_cast<uint8_t>(std::max(0, v - n));
            }
        }
    }

    void rect(int x0, int y0, int x1, int y1, uint8_t r, uint8_t g, uint8_t b) {
        if (x0 > x1) { std::swap(x0, x1); }
        if (y0 > y1) { std::swap(y0, y1); }
        for (int y = y0; y < y1; y++) {
            for (int x = x0; x < x1; x++) {
                set_pixel(x, y, r, g, b);
            }
        }
    }

    void scroll_left(int d) {
        for (int y = 0; y < m_height; y++) {
            auto l = m_data.begin() + y * m_width * 3;
            for (int i = 0; i < (m_width-d)*3; i++) {
                l[i] = l[i + d*3];
            }
        }
    }

    void scroll_up(int d) {
        for (int y = 0; y < m_height-d; y++) {
            auto l = &m_data[y * m_width * 3];
            memcpy(l, l + d * m_width * 3, d * m_width * 3);
        }
    }

    void line(int x0, int y0, int x1, int y1, uint8_t r, uint8_t g, uint8_t b) {
        int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
        int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
        int err = dx + dy;

        while (true) {
            set_pixel(x0, y0, r, g, b);
            if (x0 == x1 && y0 == y1) { break; }
            int e2 = 2 * err;
            if (e2 >= dy) {
                err += dy;
                x0 += sx;
            }
            if (e2 <= dx) {
                err += dx;
                y0 += sy;
            }
        }
    }

    int width() const { return m_width; }
    int height() const { return m_height; }
    int rand_x() const { return rand() % m_width; }
    int rand_y() const { return rand() % m_height; }
    int frame() const { return m_frame; }

private:
    int m_frame;
    std::filesystem::path m_filename_path;
    int m_width;
    int m_height;
    std::vector<uint8_t> m_data;
};

#endif // AOC2025_BASE_H