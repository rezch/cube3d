#include <bits/stdc++.h>
#include "utils.h"


struct Point3D {
    Point3D() = default;

    Point3D(double x, double y, double z) :
        x(x), y(y), z(z) { }

    Point3D(const Point3D& other) :
        x(other.x), y(other.y), z(other.z) { }

    Point3D operator + (const Point3D& o) const {
        return { x + o.x, y + o.y, z + o.z };
    }

    Point3D operator - (const Point3D& o) const {
        return { x - o.x, y - o.y, z - o.z };
    }

    Point3D operator * (double t) const {
        return { t * x, t * y, t * z };
    }

    double x;
    double y;
    double z;
};


struct Vector3D : public Point3D {
    Vector3D() = default;

    Vector3D(double x, double y, double z) :
        Point3D(x, y, z), a({}), b(x, y, z) { }

    Vector3D(const Vector3D& other) :
        Point3D(other.b - other.a), a(other.a), b(other.b) { }

    Vector3D(const Point3D& p) :
        Point3D(p), a({}), b(p) { }

    Vector3D(const Point3D& a, const Point3D& b) :
        Point3D(b - a), a(a), b(b) { }

    double scalar_mult(const Vector3D& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Vector3D vector_mult(const Vector3D& o) const {
        return Vector3D{
            y * o.z - z * o.y,
            - x * o.z + z * o.x,
            x * o.y - y * o.x,
        }.norm();
    }

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

    Point3D a;
    Point3D b;
};


struct Line {
    Line() = default;

    Line(const Point3D& o, const Vector3D& l) :
        o(o), l(l) { }

    Line(const Vector3D& l) :
        o({}), l(l) { }

    Line(const Point3D& a, const Point3D& b) :
        o(a), l(b - a) { }

    Point3D o;
    Vector3D l;
};

struct Plane {
    Vector3D norm() const { return p.vector_mult(q); }

    Point3D m;
    Vector3D p;
    Vector3D q;
};

std::optional<Point3D> intersection(const Plane& p, const Line& l, double eps = 1e-10) {
    Vector3D n = p.norm();
    if (std::abs(n.scalar_mult(l.l)) < eps) // line pendicular to plane
        return std::nullopt;

    return std::optional<Point3D>(
        l.o - l.l * (n.scalar_mult(l.o) / n.scalar_mult(l.l))
    );
}

std::optional<Vector3D> projection(const Plane& p, const Vector3D& v) {
    Vector3D n = p.norm();
    Line lA(v.a, n);
    Line lB(v.b, n);
    auto vA = intersection(p, lA);
    auto vB = intersection(p, lB);
    if (!vA || !vB)
        return std::nullopt;

    return std::optional<Vector3D>({
        vA.value(), vB.value()
    });
}

signed main() {
    Point3D M = {2, 1, 4};
    Vector3D p = {4, 1, 0};
    Vector3D q = {3, 1, 2};
    Plane pi = {M, p, q};

    Point3D A = {-3, 2, 5};
    Vector3D B = {-4, 7, 2};
    Vector3D v = {A, B};

    auto vp = projection(pi, v).value();
    write(vp.a.x, vp.a.y, vp.a.z);
    write(vp.b.x, vp.b.y, vp.b.z);
}

