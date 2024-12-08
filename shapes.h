#pragma once

#include "geometry.h"
#include "utils.h"

#include <functional>
#include <iterator>
#include <vector>


struct Shape {
    virtual ~Shape() = default;

    virtual void moveToPoint(const Point3D& to) = 0;
    virtual void rotateX(double angle) = 0;
    virtual void rotateY(double angle) = 0;
    virtual void acceptVisitor(std::function<void(const Triangle&)>&&) const = 0;
};


struct Polygon : public Triangle, public Shape {
    Polygon() = default;

    Polygon(Point3D a, Point3D b, Point3D c) : Triangle(a, b, c) { }

    void moveToPoint(const Point3D& p) override {
        a = a - p;
        b = b - p;
        c = c - p;
    }

    void rotateX(double angle) override { // TODO for point based rotation
        auto centre = getCentre();
        moveToPoint(centre);
        a.rotateX(angle);
        b.rotateX(angle);
        c.rotateX(angle);
        moveToPoint(centre * -1);
    }

    void rotateY(double angle) override { // TODO for point based rotation
        auto centre = getCentre();
        moveToPoint(centre);
        a.rotateY(angle);
        b.rotateY(angle);
        c.rotateY(angle);
        moveToPoint(centre * -1);
    }

    void acceptVisitor(std::function<void(const Triangle&)>&& visitor) const override {
        visitor(std::move(*this));
    }
};


struct Rectangle : public Shape {

    Rectangle() = default;

    // construct rect by 3 points
    Rectangle(const Point3D& a,
        const Point3D& b,
        const Point3D& c) {
        polygons_.first = { a, b, c };

        // try to find 4-th point
        if (get4thPoint(a, b, c)) return;
        if (get4thPoint(b, c, a)) return;
        if (get4thPoint(c, a, b)) return;
    }

private:
    bool get4thPoint(const Point3D& a,
        const Point3D& b,
        const Point3D& c) {
        // a    - start point
        // b, c - direction points
        Vector3D p = { a, b };
        Vector3D q = { a, c };
        if (p + q == c)
            return false;
        Point3D d = p + q;
        polygons_.second = { b, c, d };
        return true;
    }

    std::pair<Triangle, Triangle> polygons_{};
};

struct Cube : public Shape {
    Cube(const Point3D&, const Point3D&) {
        // construct cube by it's centre and one corner point
    }

    std::vector<Triangle> polygons_;
};

