#include <iostream>
#include <vector>
#include <thread>

#include "utils.h"
#include "geometry.h"
#include "shapes.h"


class Canvas {
    static constexpr auto FONT = " `.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@";

    static constexpr double EPS = 1e-8;

    static constexpr uint8_t MAX_SATURATION = 92;

public:

    Canvas() = default;

    void setSize(size_t height, size_t width) {
        height_ = height;
        width_ = width;
        matrix_ = std::vector<std::vector<uint8_t>>(height, std::vector<uint8_t>(width));
    }

    template<class Os_>
    void drawScreen(Os_& os) const {
        os << "\033[0;0H\033[2J"; // clear screen
        std::string screen(height_ * (width_ + 1), '\n');
        for (size_t i = 0; i < height_; ++i)
            for (size_t j = 0; j < width_; ++j)
                screen[i * (width_ + 1) + j] = FONT[matrix_[i][j]];
        os << screen;
    }

    void addDrawable(Triangle v) {
        drawable_.push_back(v);
    }

    double mn{}, mx{};
    double getDepth(const Triangle& polygon, double x, double y) {
        Plane p = {
            polygon.a,             // plane point
            polygon.b - polygon.a, // plane vec1
            polygon.c - polygon.a  // plane vec2
        };
        Line line = {
            Point3D{ x, y, 0 },       // start point
            Vector3D{ 0, 0, 1 } // direction
        };
        auto intersec = intersection(p, line);
        return intersec ? intersec->z : 0;
    }

    std::pair<size_t, size_t> getPolygonSegmentX(const Triangle&) const {
        // TODO: fix that
        return { 0, height_ - 1 };
    }

    std::pair<size_t, size_t> getPolygonSegmentY(const Triangle&) const {
        // TODO: fix that
        return { 0, width_ - 1 };
    }

    bool inPolygon(const Triangle& t, double x, double y) const {
        double denominator = ((t.b.y - t.c.y) * (t.a.x - t.c.x) + (t.c.x - t.b.x) * (t.a.y - t.c.y));
        double a = ((t.b.y - t.c.y) * (x - t.c.x) + (t.c.x - t.b.x) * (y - t.c.y)) / denominator;
        double b = ((t.c.y - t.a.y) * (x - t.c.x) + (t.a.x - t.c.x) * (y - t.c.y)) / denominator;
        double c = 1 - a - b;
        return a >= 0 && b >= 0 && c >= 0;
    }

    void drawPolygon(const Triangle& polygon) {
        auto [minX, maxX] = getPolygonSegmentX(polygon);
        auto [minY, maxY] = getPolygonSegmentY(polygon);
        for (size_t x = minX; x <= maxX; ++x) {
            for (size_t y = minY; y <= maxY; ++y) {
                if (!inPolygon(polygon, x, y))
                    continue;

                uint8_t layer = getDepth(polygon, x, y) * double(MAX_SATURATION - 1);

                if (layer < 0)
                    continue;

                matrix_[x][y] = std::max(
                    matrix_[x][y],
                    layer
                );
            }
        }
    }

    void refresh() {
        for (size_t x = 0; x < height_; ++x)
            for (size_t y = 0; y < width_; ++y)
                matrix_[x][y] = 0;
        // redraw polygons
        for (const auto& polygon : drawable_)
            drawPolygon(polygon);
    }

    std::vector<Triangle> drawable_;
    size_t height_;
    size_t width_;
    std::vector<std::vector<uint8_t>> matrix_;
};

signed main() {
    Triangle t = {
        { 14, 90, 0.2 },
        { 5, 5, 0.4 },
        { 30, 15, 0.4 },
    };

    Triangle t1 = {
        { 14, 90, 0.1 },
        { 30, 15, 0.3 },
        { 80, 200, 0.2 }
    };

    Canvas canvas;
    canvas.setSize(100, 300);

    canvas.addDrawable(t);
    canvas.addDrawable(t1);

    using namespace std::chrono_literals;
    for (int i = 0; i < 200; ++i) {
        canvas.drawable_[0].rotateX(0.04);
        canvas.drawable_[1].rotateY(0.08);
        canvas.refresh();
        canvas.drawScreen(std::cout);
        std::this_thread::sleep_for(10ms);
    }
    write(canvas.mn, canvas.mx);
}

