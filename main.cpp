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

Point3D intersection(const Plane& p, const Line& l, double eps = 1e-10) {
    Vector3D n = p.norm();

    double x = std::abs(l.l.x) < eps ? 0
        : ((n.y * l.l.y / l.l.x) * l.o.x - l.o.y * n.y +
        (n.z * l.l.z / l.l.x) * l.o.x - l.o.z * n.z) /
        (n.x + n.y * l.l.y / l.l.x + n.z * l.l.z / l.l.x);

    double y = std::abs(l.l.y) < eps ? 0
        : ((n.x * l.l.x / l.l.y) * l.o.y - l.o.x * n.x +
        (n.z * l.l.z / l.l.y) * l.o.y - l.o.z * n.z) /
        (n.x * l.l.x / l.l.y + n.y + n.z * l.l.z / l.l.y);

    double z = std::abs(l.l.z) < eps ? 0
        : ((n.x * l.l.x / l.l.z) * l.o.z - l.o.x * n.x +
        (n.y * l.l.y / l.l.z) * l.o.z - l.o.y * n.y) /
        (n.x * l.l.x / l.l.z + n.y * l.l.y / l.l.z + n.z);

    return { x, y, z };
}

Vector3D projection(const Plane& p, const Vector3D& v) {
    Vector3D n = p.norm();
    Line lA(v.a, n);
    Line lB(v.b, n);
    return {
        intersection(p, lA),
        intersection(p, lB),
    };
}

signed main() {
    Point3D M = {2, 1, 4};
    Vector3D p = {4, 1, 0};
    Vector3D q = {3, 1, 2};
    Plane pi = {M, p, q};

    Point3D A = {-3, 2, 5};
    Vector3D B = {-4, 7, 2};
    Vector3D v = {A, B};

    auto vp = projection(pi, v);
    write(vp.a.x, vp.a.y, vp.a.z);
    write(vp.b.x, vp.b.y, vp.b.z);
}

