#pragma once

#include <iostream>
#include <cmath>
#include <optional>


struct Point3D {
    Point3D() = default;

    Point3D(double x, double y, double z) :
        x(x), y(y), z(z) { }

    Point3D operator + (const Point3D& o) const {
        return { x + o.x, y + o.y, z + o.z };
    }

    Point3D operator - (const Point3D& o) const {
        return { x - o.x, y - o.y, z - o.z };
    }

    Point3D operator + (double t) const {
        return { x + t, y + t, z + t };
    }

    Point3D operator - (double t) const {
        return { x - t, y - t, z - t };
    }

    Point3D operator * (double t) const {
        return { t * x, t * y, t * z };
    }

    Point3D operator / (double t) const {
        return { x / t, y / t, z / t };
    }

    friend Point3D operator + (double t, const Point3D& p) {
       return p + t;
    }

    friend Point3D operator * (double t, const Point3D& p) {
       return p * t;
    }

    bool operator == (const Point3D& p) const {
        return x == p.x && y == p.y && z == p.z;
    }

    bool operator != (const Point3D& p) const {
        return !(*this == p);
    }

    void toInt() {
        x = std::round(x);
        y = std::round(y);
        z = std::round(z);
    }

    void rotateX(double angle) {
        double y_old = y;
        y = cos(angle) * y - sin(angle) * (z * 100);
        z = (cos(angle) * (z * 100) + sin(angle) * y_old) / 100; // "/100" - temp
    }

    void rotateY(double angle) {
        double x_old = x;
        x = sin(angle) * (z * 100) + cos(angle) * x;
        z = (cos(angle) * (z * 100) - sin(angle) * x_old) / 100; // "/100" - temp
    }

    double x;
    double y;
    double z;
};


struct Vector3D : public Point3D {
    Vector3D() = default;

    Vector3D(double x, double y, double z) :
        Point3D(x, y, z), a({}), b(x, y, z) { }

    Vector3D(const Point3D& p) :
        Point3D(p), a({}), b(p) { }

    Vector3D(const Point3D& a, const Point3D& b) :
        Point3D(b - a), a(a), b(b) { }

    double len() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vector3D norm() const {
        double _len = len();
        return { x / _len, y / _len, z / _len };
    }

    void toNorm() {
        double _len = len();
        x /= _len;
        y /= _len;
        z /= _len;
    }

    void toInt() {
        Point3D::toInt();
        a.toInt();
        b.toInt();
    }

    Point3D a;
    Point3D b;
};

inline double scalarMult(const Vector3D& a, const Vector3D& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vector3D vectorMult(const Vector3D& a, const Vector3D& b) {
    return {
        a.y * b.z - a.z * b.y,
        - a.x * b.z + a.z * b.x,
        a.x * b.y - a.y * b.x,
    };
}

struct Line {
    Line() = default;

    Line(const Point3D& o, const Vector3D& l) :
        o(o), l(l) { }

    Line(const Vector3D& l) :
        o({}), l(l) { }

    Line(const Point3D& a, const Point3D& b) :
        o(a), l(b - a) { }

    Line norm() const {
        auto copy = *this;
        copy.toNorm();
        return copy;
    }

    void toNorm() {
        l.toNorm();
    }

    Point3D o;
    Vector3D l;
};

struct Plane {
    Vector3D norm() const { return vectorMult(p, q); }

    Point3D m;
    Vector3D p;
    Vector3D q;
};

inline std::optional<Point3D> intersection(
        const Plane& p,
        const Line& l,
        double eps=1e-8) {
    Vector3D n = p.norm();
    Line ln = l.norm();
    if (std::abs(scalarMult(n, ln.l)) < eps)
        return std::nullopt; // line pendicular to plane

    return std::optional<Point3D>(
        ln.o + ln.l * (scalarMult(n, p.m - ln.o) / scalarMult(n, ln.l))
    );
}

inline std::optional<Vector3D> projection(
        const Plane& p,
        const Vector3D& v,
        double eps=1e-10) {
    Vector3D n = p.norm();
    Line lA(v.a, n);
    Line lB(v.b, n);
    auto vA = intersection(p, lA, eps);
    auto vB = intersection(p, lB, eps);
    return (!vA || !vB)
        ? std::nullopt
        : std::optional<Vector3D>({
            vA.value(), vB.value()
        });
}


struct Triangle {

    Triangle() = default;

    Triangle(Point3D a, Point3D b, Point3D c) : a(a), b(b), c(c) { }

    Point3D getCentre() const {
        return (a + b + c) / 3;
    }

    Vector3D a;
    Vector3D b;
    Vector3D c;
};
