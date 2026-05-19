#pragma once

#include <cmath>
#include <iostream>

template<typename T, size_t N>
class Vec;

using Vec2 = Vec<float, 2>;
using Vec3 = Vec<float, 3>;
using Vec4 = Vec<float, 4>;

using Pixel = Vec<int, 2>;
using Color = Vec<float, 4>;

template<typename Derived, size_t N>
class VecMath {
protected:
	Derived& self() { return static_cast<Derived&>(*this); }
	const Derived& self() const { return static_cast<const Derived&>(*this); }

public:
	VecMath() = default;

	template<typename T2, size_t N2>
    VecMath(const Vec<T2, N2>& otherVec) {
        constexpr size_t count = (N < N2) ? N : N2;

        for (size_t i = 0; i < count; i++) {
            self().data[i] = otherVec.data[i];
        }

        if (N == 4)
            self().data[3] = 1;
    }

    template<typename T2, size_t N2>
    Derived& operator=(const Vec<T2, N2>& otherVec ) {
        constexpr size_t count = (N < N2) ? N : N2;

        for (size_t i = 0; i < count; i++) {
            self().data[i] = otherVec.data[i];
        }

        return self();
    }

    Derived operator+(const Derived& otherVec) const {
        Derived result;

        for (size_t i = 0; i < N; i++) {
            result.data[i] = self().data[i] + otherVec.data[i];
        }

        return result;
    }

    template<typename T2, size_t N2>
    Derived& operator+=(const Vec<T2, N2>& otherVec) {
        for (size_t i = 0; i < N; i++) {
            self().data[i] += otherVec.data[i];
        }

        return self();
    }


    Derived operator-(const Derived& otherVec) const {
        Derived result;

        for (size_t i = 0; i < N; i++) {
            result.data[i] = self().data[i] - otherVec.data[i];
        }

        return result;
    }

    template<typename T2, size_t N2>
    Derived& operator-=(const Vec<T2, N2>& otherVec) {
        for (size_t i = 0; i < N; i++) {
            self().data[i] -= otherVec.data[i];
        }

        return self();
    }

    Derived operator-() const {
        Derived result;

        for (int i = 0; i < N; i++) {
            result.data[i] = -self().data[i];
        }

        return result;
    }

    Derived operator*(const float scalar) const {
        Derived result;
        	
        for (size_t i = 0; i < N; i++) {
            result.data[i] = self().data[i] * scalar;
        }

        return result;
    }

    float operator*(const Derived& otherVec) const {
        float result = 0;

        for (size_t i = 0; i < N; i++) {
            result += self().data[i] * otherVec.data[i];
        }

        return result;
    }

    Derived operator*=(const float scalar) {
        return self() = self() * scalar;
    }

    double length() const {
        double lengthSquared = 0;

        for (int i = 0; i < N; i++) {
            lengthSquared += std::pow(self().data[i], 2);
        }

        return std::pow(lengthSquared, 0.5);
    }

    Derived operator*=(const Derived& otherVec) {
        for (size_t i = 0; i < N; i++) {
            self().data[i] = self().data[i] * otherVec.data[i];
        }
        return self();
    }

    Derived operator/(const float scalar) {
        Derived result;

        for (size_t i = 0; i < N; i++) {
            result.data[i] = self().data[i] / scalar;
        }

        return result;
    }

    Derived operator/=(const float scalar) {
        for (size_t i = 0; i < N; i++) {
            self().data[i] = self().data[i] / scalar;
        }

        return self();
    }

    Derived unit() const {
        Derived unit;
        double vecLength = length();
		for (int i = 0; i < N; i++) {
			unit.data[i] += self().data[i] / vecLength;
		}

		return unit;
	}

	void print() const{
		for (size_t i = 0; i < N; i++) {
			std::cout << self().data[i] << ", ";
		}
		std::cout << std::endl;
	}
};


template<typename T>
class Vec<T, 2> : public VecMath<Vec<T,2>, 2>{
public:
	union {
		T data[2];
		struct { T x, y; };
		struct { T u, v; };
	};

	using value_type = T;
	static constexpr size_t size = 2;

public:
	constexpr Vec() : x(0), y(0) {};
	constexpr Vec(T x, T y) : x(x), y(y) {};

	template<typename T2, size_t N2>
	constexpr Vec(const Vec<T2, N2>& otherVec) : VecMath<Vec<T, 2>, 2>(otherVec) {}

	using VecMath<Vec<T, 2>, 2>::operator=;
};

template<typename T>
class Vec<T, 3> : public VecMath<Vec<T, 3>, 3> {
public:
	union {
		T data[3];
		struct { T x, y, z; };
		struct { T r, g, b; };
		struct { T alfa, beta, gama;  };
	};

	using value_type = T;
	static constexpr size_t size = 3;

public:
	constexpr Vec() : x(0), y(0), z(0) {};
	constexpr Vec(T x, T y, T z) : x(x), y(y), z(z) {};

	template<typename T2, size_t N2>
	constexpr Vec(const Vec<T2, N2>& otherVec) : VecMath<Vec<T, 3>, 3>(otherVec) {}

	using VecMath<Vec<T, 3>, 3>::operator=;
};

template<typename T>
class Vec<T, 4> : public VecMath<Vec<T, 4>, 4> {
public:
	union {
		T data[4];
		struct { T x, y, z, w; };
		struct { T r, g, b, a; };
	};

	using value_type = T;
	static constexpr size_t size = 4;

public:
	constexpr Vec() : x(0), y(0), z(0), w(0) {};
	constexpr Vec(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {};

	template<typename T2, size_t N2>
	constexpr Vec(const Vec<T2, N2>& otherVec) : VecMath<Vec<T, 4>, 4>(otherVec) {}

	using VecMath<Vec<T, 4>, 4>::operator=;
};

inline Vec3 cross(const Vec3& a, const Vec3& b) {
	Vec3 result;

	result.x = a.y * b.z - a.z * b.y;
	result.y = a.z * b.x - a.x * b.z;
	result.z = a.x * b.y - a.y * b.x;

	return result;
}