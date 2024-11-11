#if defined(__APPLE__)
    #include <iostream>
    #include <cmath>
    #include <optional>
#else
    #include <bits/stdc++.h>
#endif


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

    void toInt() {
        x = std::round(x);
        y = std::round(y);
        z = std::round(z);
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

double scalar_mult(const Vector3D& a, const Vector3D& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3D vector_mult(const Vector3D& a, const Vector3D& b) {
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

    Point3D o;
    Vector3D l;
};

struct Plane {
    Vector3D norm() const { return vector_mult(p, q); }

    Point3D m;
    Vector3D p;
    Vector3D q;
};

std::optional<Point3D> intersection(
        const Plane& p,
        const Line& l,
        double eps=1e-10) {
    Vector3D n = p.norm();
    if (std::abs(scalar_mult(n, l.l)) < eps)
        return std::nullopt; // line pendicular to plane 

    return std::optional<Point3D>(
        l.o - l.l * (scalar_mult(n, p.m - l.o) / scalar_mult(n, l.l))
    );
}

std::optional<Vector3D> projection(
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
    Vector3D a;
    Vector3D b;
    Vector3D c;
};


