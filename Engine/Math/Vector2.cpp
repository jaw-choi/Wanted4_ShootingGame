#include "Vector2.h"
#include <iostream>

namespace Wanted
{
	Vector2 Vector2::Zero(0, 0);
	Vector2 Vector2::One(1, 1);
	Vector2 Vector2::Up(0, 1);
	Vector2 Vector2::Right(1, 0);

	Vector2::Vector2()
	{
	}

	Vector2::Vector2(int x, int y)
		: x(x), y(y)
	{
	}
	
	Vector2::~Vector2()
	{
		if (string)
		{
			delete[] string;
			string = nullptr;
		}
	}
	
	const char* Vector2::ToString()
	{
		// 기존 문자열이 있다면 제거.
		if (string)
		{
			delete[] string;
			string = nullptr;
		}

		string = new char[128];
		memset(string, 0, sizeof(char) * 128);
		sprintf_s(string, 128, "(%d, %d)", x, y);

		return string;
	}

	Vector2 Vector2::operator+(const Vector2& other) const
	{
		return Vector2(x + other.x, y + other.y);
	}

	Vector2 Vector2::operator-(const Vector2& other) const
	{
		return Vector2(x - other.x, y - other.y);
	}

	bool Vector2::operator==(const Vector2& other) const
	{
		return (x == other.x) && (y == other.y);
	}

	bool Vector2::operator!=(const Vector2& other) const
	{
		return !(*this == other);
	}

	Vector2::operator COORD() const
	{
		COORD coord = {};
		coord.X = static_cast<short>(x);
		coord.Y = static_cast<short>(y);

		return coord;
	}
	//--------------------------------------------------------------------------------------------------------/
	//--------------------------------------------------------------------------------------------------------/
	//---------------------------------------vector2 float ---------------------------------------------------/
	//--------------------------------------------------------------------------------------------------------/
	//--------------------------------------------------------------------------------------------------------/

	Vector2f::Vector2f()
	    : x(0.0f), y(0.0f)
	{
	}

	Vector2f::Vector2f(float x, float y)
	    : x(x), y(y)
	{
	}

	Vector2f::~Vector2f()
	{
	}

	Vector2f::Vector2f(const Vector2& v)
	    : x(static_cast<float>(v.x)),
	    y(static_cast<float>(v.y))
	{
	}


	float Vector2f::LengthSquared() const
	{
	    return x * x + y * y;
	}

	Vector2f Vector2f::Normalized() const
	{
	    float lenSq = LengthSquared();
	    if (lenSq <= 0.0f)
		return Vector2f(0.0f, 0.0f);

	    float invLen = 1.0f / std::sqrt(lenSq);
	    return Vector2f(x * invLen, y * invLen);
	}

	Vector2f Vector2f::operator+(const Vector2f& rhs) const
	{
	    return Vector2f(x + rhs.x, y + rhs.y);
	}

	Vector2f Vector2f::operator-(const Vector2f& rhs) const
	{
	    return Vector2f(x - rhs.x, y - rhs.y);
	}

	bool Vector2f::operator==(const Vector2f& other) const
	{
	    // 부동소수점 비교는 오차 허용
	    const float eps = 0.00001f;
	    return std::fabs(x - other.x) < eps &&
		std::fabs(y - other.y) < eps;
	}

	bool Vector2f::operator!=(const Vector2f& other) const
	{
	    return !(*this == other);
	}
}