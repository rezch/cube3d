#include <bits/stdc++.h>

#include "utils.h"
#include "geometry.h"


class Canvas {
    static constexpr auto FONT = " `.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@";

public:

    static constexpr int8_t MAX_SATURATION = 92;

    Canvas() = default;

    void setSize(size_t height, size_t width) {
        height_ = height;
        width_ = width;
        matrix_ = std::vector<std::vector<int8_t>>(height, std::vector<int8_t>(width));
    }

    template<class Os_>
    void drawScreen(Os_& os) const {
        for (const auto& line : matrix_) {
            for (int8_t pixel : line) os << FONT[pixel];
            os << '\n';
        }
    }

    void addDrawable(Triangle v) {
        drawable_.push_back(v);
    }

    std::pair<int, int> redraw(size_t x, const Triangle& polygon) const {
        std::vector<std::pair<Vector3D, Vector3D>> sides = {
            { polygon.a, polygon.b },
            { polygon.a, polygon.c },
            { polygon.b, polygon.c },
        };
        std::pair<int, int> result = { -1, -1 };
        auto addResult = [&result](int y) {
            if (result.first == -1)
                result.first = y;
            else
                result.second = y;
        };
        for (auto& side : sides) {
            side.first.toInt();
            side.second.toInt();
            if (side.first.x > side.second.x)
                std::swap(side.first, side.second);
            if (side.first.x > x || side.second.x < x)
                continue;
            if (side.first.x == side.second.x)
                addResult(x);
            double k = (side.first.y - side.second.y) / (side.first.x - side.second.x);
            addResult(
                k * x + side.first.y - k * side.first.x
            );
        }
        return result;
    }

    auto scanline() const {
        std::vector<std::vector<std::pair<size_t, size_t>>> result(height_);
        for (size_t x = 0; x < height_; ++x) {
            for (auto& polygon : drawable_) {
                auto segment = redraw(x, polygon);
                result[x].push_back({ segment.first, -1 });
                result[x].push_back({ segment.second, 1 });
            }
            std::sort(result[x].begin(), result[x].end());
        }
        return result;
    }

    void redrawScreen() {
        auto segments = scanline();
        for (size_t x = 0; x < height_; ++x) {
            size_t layer{};
            auto curr = segments[x].begin();
            for (size_t y = 0; y < width_; ++y) {
                while (curr < segments[x].end() && curr->first < y) {
                    layer += (curr++)->second;
                }
                if (curr->first != y) {
                    matrix_[x][y] = layer;
                    continue;
                }
            }
        }
    }

    std::vector<Triangle> drawable_;
    size_t height_;
    size_t width_;
    std::vector<std::vector<int8_t>> matrix_;
};

signed main() {
    Vector3D a = { 4, 90, 0 };
    Vector3D b = { 1, 0, 0 };
    Vector3D c = { 30, 15, 0 };
    Triangle t = { a, b, c };

    Canvas canvas;
    canvas.setSize(36, 120);
    canvas.addDrawable(t);
    canvas.redrawScreen();
    canvas.drawScreen(std::cout);
}

