#pragma once

#include <vector>

#include "geometry.h"


struct Shape {

    ~Shape() = default;

    virtual void move(const Point3D& to) = 0;

    virtual void rotateX(double angle) = 0;

    virtual void rotateY(double angle) = 0;
};

struct Triangle : public Shape {

    Triangle() = default;

    Triangle(const Point3D& a,
        const Point3D& b,
        const Point3D& c)
        : a(a), b(b), c(c) { }

    Point3D getCentre() const {
        return (a + b + c) / 3;
    }

    void move(const Point3D& p) override {
        a = a - p;
        b = b - p;
        c = c - p;
    }

    void rotateX(double angle) override {
        auto centre = getCentre();
        move(centre);
        a.rotateX(angle);
        b.rotateX(angle);
        c.rotateX(angle);
        move(centre * -1);
    }

    void rotateY(double angle) override {
        auto centre = getCentre();
        move(centre);
        a.rotateY(angle);
        b.rotateY(angle);
        c.rotateY(angle);
        move(centre * -1);
    }

    Vector3D a;
    Vector3D b;
    Vector3D c;
};


struct Rectangle : public Shape {

    Rectangle() = default;

    // construct rect by 3 points
    Rectangle(const Point3D& a,
        const Point3D& b,
        const Point3D& c) {
        polygons_.first = { a, b, c };

        // try to find 4th point
        if (get4thPoint(a, b, c)) return;
        if (get4thPoint(b, a, c)) return;
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

