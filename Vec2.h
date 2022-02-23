#ifndef VEC2_H_ 
#define VEC2_H_

#include <cmath>
#include <math.h>
#include <iostream>

template<typename T> class Vec2;

template <class T> bool operator!= (const Vec2<T> & p1, const Vec2<T> & p2) {
    return (p1.x != p2.x || p1.y != p2.y);
}

template <class T> const Vec2<T> operator* (const Vec2<T> & p, float factor) {
    return Vec2<T> (p.x * factor, p.y * factor);
}

template <class T> const Vec2<T> operator* (float factor, const Vec2<T> & p) {
    return Vec2<T> (p.x * factor, p.y * factor);
}

template <class T> const Vec2<T> operator* (const Vec2<T> & p1, const Vec2<T> & p2) {
    return Vec2<T> (p1.x * p2.x, p1.y * p2.y);
}

template <class T> const Vec2<T> operator+ (const Vec2<T> & p1, const Vec2<T> & p2) {
    return Vec2<T> (p1.x + p2.x, p1.y + p2.y);
}

template <class T> const Vec2<T> operator- (const Vec2<T> & p1, const Vec2<T> & p2) {
    return Vec2<T> (p1.x - p2.x, p1.y - p2.y);
}

template <class T> const Vec2<T> operator+ (const Vec2<T> & p1, float f) {
    return Vec2<T> (p1.x + f, p1.y + f);
}

template <class T> const Vec2<T> operator- (const Vec2<T> & p1, float f) {
    return Vec2<T> (p1.x - f, p1.y - f);
}

template <class T> const Vec2<T> operator- (const Vec2<T> & p) {
    return Vec2<T> (-p.x, -p.y);
}

template <class T> const Vec2<T> operator/ (const Vec2<T> & p, float divisor) {
    return Vec2<T> (p.x/divisor, p.y/divisor);
}

template <class T> bool operator== (const Vec2<T> & p1, const Vec2<T> & p2) {
    return (p1.x == p2.x && p1.y == p2.y);
}

template <class T> bool operator< (const Vec2<T> & a, const Vec2<T> & b) {
    return (a.x < b.x && a.y < b.y);
}

template <class T> bool operator>= (const Vec2<T> & a, const Vec2<T> & b) {
    return (a.x >= b.x || a.y >= b.y);
}


/**
 * Vector in 2 dimensions, with basics operators overloaded.
 */
template <typename T>
class Vec2 {
public:
    inline Vec2 (void)	{
        x = y = T ();
    }
    inline Vec2 (T p) {
        x = p;
        y = p;
    };
    inline Vec2 (T p0, T p1) {
        x = p0;
        y = p1;
    };
    inline Vec2 (const Vec2 & v) {
        init (v.x, v.y);
    }
    inline Vec2 (T* pp) {
        x = pp.x;
        y = pp.y;
    };
    // ---------
    // Operators
    // ---------
    inline T& operator[] (int Index) {
        return (p[Index]);
    };
    inline const T& operator[] (int Index) const {
        return (p[Index]);
    };
    inline Vec2& operator= (const Vec2 & P) {
        x = P.x;
        y = P.y;
        return (*this);
    };
    inline Vec2& operator+= (const Vec2 & P) {
        x += P.x;
        y += P.y;
        return (*this);
    };
    inline Vec2& operator-= (const Vec2 & P) {
        x -= P.x;
        y -= P.y;
        return (*this);
    };
    inline Vec2& operator*= (const Vec2 & P) {
        x *= P.x;
        y *= P.y;
        return (*this);
    };
    inline Vec2& operator*= (T s) {
        x *= s;
        y *= s;
        return (*this);
    };
    inline Vec2& operator/= (const Vec2 & P) {
        x /= P.x;
        y /= P.y;
        return (*this);
    };
    inline Vec2& operator/= (T s) {
        x /= s;
        y /= s;
        return (*this);
    };

    //---------------------------------------------------------------

    inline Vec2 & init (T x, T y) {
        this->x = x;
        this->y = y;
        return (*this);
    };
    inline T getSquaredLength() const {
        return (dot(*this, *this));
    };
    inline T getLength() const {
        return (T)sqrt (getSquaredLength());
    };
    /// Return length after normalization
    inline Vec2 normalize () {
        T length = getLength();
        if (length == 0.0f)
            return Vec2(T());
        return Vec2(x, y) / length;
    };
    inline void fromTo (const Vec2 & P1, const Vec2 & P2) {
        x = P2.x - P1.x;
        y = P2.y - P1.y;
    };
    inline float transProduct (const Vec2 & v) const {
        return (x*v.x + y*v.y);
    }
    static inline Vec2 getOrthogonal (const Vec2& v) {
        return Vec2(v.y, -v.x);
    }
    static inline Vec2 segment (const Vec2 & a, const Vec2 & b) {
        Vec2 r;
        r.x = b.x - a.x;
        r.y = b.y - a.y;
        return r;
    };
    static inline T dot(const Vec2 & a, const Vec2 & b) {
        return (a.x * b.x + a.y * b.y);
    }
    static inline T squaredDistance (const Vec2 &v1, const Vec2 &v2) {
        Vec2 tmp = v1 - v2;
        return (tmp.getSquaredLength());
    }
    static inline T distance (const Vec2 &v1, const Vec2 &v2) {
        Vec2 tmp = v1 - v2;
        return (tmp.getLength());
    }
    static inline Vec2 interpolate (const Vec2 & u, const Vec2 & v, T alpha) {
        return (u * (1.0f - alpha) + v * alpha);
    }

    // https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
    static bool isIntersect(const Vec2& p1, const Vec2& q1, const Vec2& p2, const Vec2& q2) {
        // Find the four orientations needed for general and special cases
        int o1 = orientation(p1, q1, p2);
        int o2 = orientation(p1, q1, q2);
        int o3 = orientation(p2, q2, p1);
        int o4 = orientation(p2, q2, q1);

        // General case
        if (o1 != o2 && o3 != o4)
            return true;

        // Special Cases
        // p1, q1 and p2 are collinear and p2 lies on segment p1q1
        if (o1 == 0 && onSegment(p1, p2, q1)) return true;

        // p1, q1 and q2 are collinear and q2 lies on segment p1q1
        if (o2 == 0 && onSegment(p1, q2, q1)) return true;

        // p2, q2 and p1 are collinear and p1 lies on segment p2q2
        if (o3 == 0 && onSegment(p2, p1, q2)) return true;

        // p2, q2 and q1 are collinear and q1 lies on segment p2q2
        if (o4 == 0 && onSegment(p2, q1, q2)) return true;

        return false; // Doesn't fall in any of the above cases
    }

	T * pointer()
	{
		return p;
	}

	const T * pointer() const
	{
		return p;
	}

    union
    {
        struct
        {
            T x, y;  // standard names for components
        };
        T p[2];  // array access
    };

private:
    static bool onSegment(const Vec2& p, const Vec2& q, const Vec2& r)
    {
        if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
            q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
            return true;

        return false;
    }

    static int orientation(const Vec2& p, const Vec2& q, const Vec2& r)
    {
        int val = int((q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y));

        if (val == 0) return 0;  // collinear

        return (val > 0) ? 1 : 2; // clock or counterclock wise
    }
};

template <class T> inline Vec2<T> swap (Vec2<T> & P, Vec2<T> & Q) {
    Vec2<T> tmp = P;
    P = Q;
    Q = tmp;
}

template <class T> std::ostream & operator<< (std::ostream & output, const Vec2<T> & v) {
    output << v.x << " " << v.y;
    return output;
}

template <class T> std::istream & operator>> (std::istream & input, Vec2<T> & v) {
    input >> v.x >> v.y;
    return input;
}

typedef Vec2<float> Vec2f;
typedef Vec2<double> Vec2d;
typedef Vec2<int> Vec2i;

#endif  // VEC2_H_