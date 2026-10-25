#ifndef VEC_H
#define VEC_H
#include <math.h>

#include <cmath>

struct Vec3 {
    float x, y, z;
    
    Vec3(float x_=0, float y_=0, float z_=0): x(x_), y(y_), z(z_) {}
    
    Vec3 operator+(const Vec3& rhs) const {
        return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
    }
    
    Vec3& operator+=(const Vec3& rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
	}
    
    Vec3 operator-(const Vec3& o) const { 
        return Vec3(x-o.x, y-o.y, z-o.z); 
    }
    
    float dot(const Vec3& o) const { 
        return x*o.x + y*o.y + z*o.z; 
    }
    
    float length() const { 
        return sqrtf(x*x + y*y + z*z); 
    }
    
    Vec3 operator*(float s) const {
        return Vec3(x * s, y * s, z * s);
    }
    
    Vec3& operator*=(float s) {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }
};

inline Vec3 operator*(float s, const Vec3& v) {
    return v * s;  
}


struct Vec2 {
    float x;
    float y;

    Vec2() : x(0.0f), y(0.0f) {}
    Vec2(float x_, float y_) : x(x_), y(y_) {}
    Vec2 operator+(const Vec2& other) const {
        return Vec2(x + other.x, y + other.y);
    }
    Vec2& operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vec2 operator-(const Vec2& other) const {
        return Vec2(x - other.x, y - other.y);
    }
    Vec2& operator-=(const Vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vec2 operator*(float scalar) const {
        return Vec2(x * scalar, y * scalar);
    }
    Vec2& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vec2 operator/(float scalar) const {
        return Vec2(x / scalar, y / scalar);
    }
    Vec2& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    bool operator==(const Vec2& other) const {
        return x == other.x && y == other.y;
    }
    bool operator!=(const Vec2& other) const {
        return !(*this == other);
    }

    float length() const {
        return std::sqrt(x * x + y * y);
    }
};

inline Vec2 operator*(float scalar, const Vec2& vec) {
    return Vec2(vec.x * scalar, vec.y * scalar);
}
#endif //VEC_H
