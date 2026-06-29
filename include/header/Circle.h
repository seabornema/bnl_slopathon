#ifndef CIRCLE_H
#define CIRCLE_H

#include <cmath>

enum class CircleMaterial
{
    ConcreteRubble,
    GraphiteRodEnd
};

struct Vec2
{
    float x;
    float y;

    Vec2()
        : x(0.0f), y(0.0f)
    {
    }

    Vec2(float xValue, float yValue)
        : x(xValue), y(yValue)
    {
    }

    Vec2 operator+(const Vec2& other) const
    {
        return Vec2(x + other.x, y + other.y);
    }

    Vec2 operator-(const Vec2& other) const
    {
        return Vec2(x - other.x, y - other.y);
    }

    Vec2 operator*(float scalar) const
    {
        return Vec2(x * scalar, y * scalar);
    }

    Vec2 operator/(float scalar) const
    {
        return Vec2(x / scalar, y / scalar);
    }

    Vec2& operator+=(const Vec2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vec2& operator-=(const Vec2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vec2& operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    float length() const
    {
        return std::sqrt(x * x + y * y);
    }

    float lengthSquared() const
    {
        return x * x + y * y;
    }

    float dot(const Vec2& other) const
    {
        return x * other.x + y * other.y;
    }

    Vec2 normalized() const
    {
        float len = length();

        if (len <= 0.0001f)
        {
            return Vec2(0.0f, 0.0f);
        }

        return Vec2(x / len, y / len);
    }
};

struct Circle
{
    Vec2 position;
    Vec2 velocity;

    float radius;
    float mass;

    CircleMaterial material;

    Circle()
        : position(),
          velocity(),
          radius(1.0f),
          mass(1.0f),
          material(CircleMaterial::ConcreteRubble)
    {
    }

    Circle(const Vec2& startPosition,
           const Vec2& startVelocity,
           float circleRadius,
           float circleMass,
           CircleMaterial circleMaterial)
        : position(startPosition),
          velocity(startVelocity),
          radius(circleRadius),
          mass(circleMass),
          material(circleMaterial)
    {
    }
};

#endif
