#include "geometry.h"
#include "shapes.h"

#include <functional>
#include <memory>
#include <thread>
#include <vector>

#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/screen/screen.hpp>


class Canvas : public ftxui::Node {
    static constexpr double EPS         = 1e-8;
    static constexpr int MAX_SATURATION = 255;

public:
    Canvas() = default;

    void ComputeRequirement() override
    {
        requirement_.min_x = 3;
        requirement_.min_y = 3;
    }

    void Render(ftxui::Screen &screen) override
    {
        for (const auto &object : drawable_) {
            object->acceptVisitor(screen, drawPolygon);
        }
    }

    void addDrawable(std::shared_ptr<Shape> &&object)
    {
        drawable_.push_back(std::move(object));
    }

    std::shared_ptr<Shape> getDrawable(size_t index)
    {
        return drawable_[index];
    }

private:
    static double getDepth(const Triangle &polygon,
                           double x,
                           double y)
    {
        Plane p = {
            // surface of polygon
            polygon.a,             // plane point
            polygon.b - polygon.a, // plane vec1
            polygon.c - polygon.a  // plane vec2
        };
        Line line = {
            // ray from screen
            Point3D{ x, y, 0 }, // start point
            Vector3D{ 0, 0, 1 }  // direction
        };
        auto intersec = intersection(p, line);
        return intersec ? intersec->z : 0;
    }

    static std::pair<size_t,
                     size_t>
    getPolygonSegmentX(const ftxui::Screen &screen,
                       const Triangle &p)
    {
        std::pair<double, double> res = {
            std::min(p.a.x, std::min(p.b.x, p.c.x)),
            std::max(p.a.x, std::max(p.b.x, p.c.x))
        };

        res.first  = std::min(res.first, (double)screen.dimx());
        res.second = std::min(res.second, (double)screen.dimx());
        res.first  = std::max(res.first, .0);
        res.second = std::max(res.second, .0);

        return res;
    }

    static std::pair<size_t,
                     size_t>
    getPolygonSegmentY(const ftxui::Screen &screen,
                       const Triangle &p)
    {
        std::pair<double, double> res = {
            std::min(p.a.y, std::min(p.b.y, p.c.y)),
            std::max(p.a.y, std::max(p.b.y, p.c.y))
        };

        res.first  = std::min(res.first, (double)screen.dimy());
        res.second = std::min(res.second, (double)screen.dimy());
        res.first  = std::max(res.first, .0);
        res.second = std::max(res.second, .0);

        return res;
    }

    static bool inPolygon(const Triangle &t,
                          double x,
                          double y)
    {
        double denominator = ((t.b.y - t.c.y) * (t.a.x - t.c.x) + (t.c.x - t.b.x) * (t.a.y - t.c.y));
        double a           = ((t.b.y - t.c.y) * (x - t.c.x) + (t.c.x - t.b.x) * (y - t.c.y)) / denominator;
        double b           = ((t.c.y - t.a.y) * (x - t.c.x) + (t.a.x - t.c.x) * (y - t.c.y)) / denominator;
        double c           = 1 - a - b;
        return a >= 0 && b >= 0 && c >= 0;
    }

    static void drawPolygon(
        ftxui::Screen &screen,
        const Triangle &polygon)
    {
        auto [minX, maxX] = getPolygonSegmentX(screen, polygon);
        auto [minY, maxY] = getPolygonSegmentY(screen, polygon);
        for (size_t x = minX; x <= maxX; ++x) {
            for (size_t y = minY; y <= maxY; ++y) {
                if (!inPolygon(polygon, x, y))
                    continue;

                double depth = getDepth(polygon, x, y) * MAX_SATURATION;
                if (depth < 0)
                    continue;

                auto &pixel = screen.PixelAt(x, y);

                pixel.background_color =
                    ftxui::Color::HSV(
                        120, // aquamarine
                        MAX_SATURATION,
                        depth);
            }
        }
    }

    std::vector<std::shared_ptr<Shape>> drawable_;
    size_t height_;
    size_t width_;
};

signed main()
{
    using namespace std::chrono_literals;

    bool game_looped = true;

    Polygon t = {
        { 20, 35, 0.2 },
        { 10, 15, 0.4 },
        { 40, 20, 0.4 },
    };

    Polygon t1 = {
        {  60, 30, 0.4 },
        { 120, 15, 0.3 },
        { 160, 40, 0.2 }
    };

    auto canvas = std::make_shared<Canvas>();

    canvas->addDrawable(std::make_shared<Polygon>(t));
    canvas->addDrawable(std::make_shared<Polygon>(t1));

    auto screen = ftxui::ScreenInteractive::Fullscreen();

    auto renderer =
        ftxui::Renderer(
            [&canvas]
            { return canvas; })
        | ftxui::border;

    auto component = CatchEvent(
        renderer,
        [&](ftxui::Event event)
        {
            if (event == ftxui::Event::Character('q')
                || event == ftxui::Event::Escape) {
                game_looped = false;
                screen.ExitLoopClosure()();
                return true;
            }
            return false;
        });

    std::thread update_thread(
        [&screen, &game_looped, &canvas]
        {
            while (game_looped) {
                canvas->getDrawable(0)->rotateX(0.02);
                canvas->getDrawable(1)->rotateY(0.04);
                canvas->Render(screen);
                screen.RequestAnimationFrame();
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(10ms));
            }
        });

    screen.Loop(component);
    update_thread.join();
}
