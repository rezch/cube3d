#include <functional>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

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
        os << "\033[0;0H\033[2J"; // clear screen (by moving cursor)
        std::string screen(height_ * (width_ + 1), '\n');
        for (size_t i = 0; i < height_; ++i)
            for (size_t j = 0; j < width_; ++j)
                screen[i * (width_ + 1) + j] = FONT[matrix_[i][j]];
        os << screen;
    }

    void addDrawable(std::unique_ptr<Shape>&& object) {
        drawable_.push_back(std::move(object));
    }

    void refresh() {
        // clear screen
        for (size_t x = 0; x < height_; ++x)
            for (size_t y = 0; y < width_; ++y)
                matrix_[x][y] = 0;

        // redraw polygons
        for (const auto& object : drawable_) {
            object->acceptVisitor(
                std::bind(
                    drawPolygon,
                    &matrix_,
                    std::placeholders::_1
                )
            );
        }
    }

// private:

    static double getDepth(const Triangle& polygon, double x, double y) {
        Plane p = { // surface of polygon
            polygon.a,             // plane point
            polygon.b - polygon.a, // plane vec1
            polygon.c - polygon.a  // plane vec2
        };
        Line line = { // ray from screen
            Point3D{ x, y, 0 },    // start point
            Vector3D{ 0, 0, 1 }    // direction
        };
        auto intersec = intersection(p, line);
        return intersec ? intersec->z : 0;
    }

    static std::pair<size_t, size_t> getPolygonSegmentX(const Triangle& p) {
        return {
            (size_t)std::min(0., std::min(p.a.x, std::min(p.b.x, p.c.x))),
            (size_t)std::max(1., std::max(p.a.x, std::max(p.b.x, p.c.x)))
        };
    }

    static std::pair<size_t, size_t> getPolygonSegmentY(const Triangle& p) {
        return {
            (size_t)std::min(0., std::min(p.a.y, std::min(p.b.y, p.c.y))),
            (size_t)std::max(1., std::max(p.a.y, std::max(p.b.y, p.c.y)))
        };
    }

    static bool inPolygon(const Triangle& t, double x, double y) {
        double denominator = ((t.b.y - t.c.y) * (t.a.x - t.c.x) + (t.c.x - t.b.x) * (t.a.y - t.c.y));
        double a = ((t.b.y - t.c.y) * (x - t.c.x) + (t.c.x - t.b.x) * (y - t.c.y)) / denominator;
        double b = ((t.c.y - t.a.y) * (x - t.c.x) + (t.a.x - t.c.x) * (y - t.c.y)) / denominator;
        double c = 1 - a - b;
        return a >= 0 && b >= 0 && c >= 0;
    }

    static void drawPolygon(std::vector<std::vector<uint8_t>>* matrix,
            const Triangle& polygon) {
        auto [minX, maxX] = getPolygonSegmentX(polygon);
        auto [minY, maxY] = getPolygonSegmentY(polygon);
        for (size_t x = minX; x <= maxX; ++x) {
            for (size_t y = minY; y <= maxY; ++y) {
                if (!inPolygon(polygon, x, y))
                    continue;

                uint8_t layer = getDepth(polygon, x, y) * double(MAX_SATURATION - 1);

                if (layer < 0)
                    continue;

                (*matrix)[x][y] = std::max(
                    (*matrix)[x][y],
                    layer
                );
            }
        }
    }

    std::vector<std::unique_ptr<Shape>> drawable_;
    size_t height_;
    size_t width_;
    std::vector<std::vector<uint8_t>> matrix_;
};

signed main() {
    Polygon t = {
        { 14, 90, 0.2 },
        { 5, 5, 0.4 },
        { 30, 15, 0.4 },
    };

    Polygon t1 = {
        { 14, 70, 0.4 },
        { 30, 15, 0.3 },
        { 80, 80, 0.2 }
    };

    Canvas canvas;
    canvas.setSize(100, 300);

    canvas.addDrawable(std::make_unique<Polygon>(t));
    canvas.addDrawable(std::make_unique<Polygon>(t1));

    using namespace std::chrono_literals;
    for (int i = 0; i < 200; ++i) {
        canvas.drawable_[0]->rotateX(0.04);
        canvas.drawable_[1]->rotateY(0.08);
        canvas.refresh();
        canvas.drawScreen(std::cout);
        std::this_thread::sleep_for(10ms);
    }
}
