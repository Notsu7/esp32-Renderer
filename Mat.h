#pragma once

#include "Vec.h"

template<typename T, size_t N>
class Mat;
#ifdef PI
#undef PI
#endif
constexpr float PI = 3.14159265358979323846f;
using Mat2 = Mat<float, 2>;
using Mat3 = Mat<float, 3>;
using Mat4 = Mat<float, 4>;

constexpr float toRad(float degrees);
constexpr float toDeg(float radians);

template<typename Derived>
class MatMath {
protected:
	Derived& self() { return static_cast<Derived&>(*this); }
	const Derived& self() const { return static_cast<const Derived&>(*this); }

public:
	MatMath() = default;

	template<typename T2, size_t N2>
	MatMath(const Mat<T2, N2>& otherMat){
		size_t count = (Derived::size < N2) ? Derived::size : N2;

		for (size_t row = 0; row < count; row++) {
			for (size_t column = 0; column < count; column++) {
				self().data[row][column] = otherMat.data[row][column];
			}
		}
	}

	template<typename T2, size_t N2>
	Derived& operator=(const Mat<T2, N2>& otherMat) {
		size_t count = (Derived::size < N2) ? Derived::size : N2;

		for (size_t row = 0; row < count; row++) {
			for (size_t column = 0; column < count; column++) {
				self().data[row][column] = otherMat.data[row][column];
			}
		}

		return self();
	}

	template<typename T2, size_t N2>
	Vec<T2, N2> operator*(const Vec<T2, N2>& vec) const {
		Vec<T2, Derived::size> result;
		Vec<T2, Derived::size> vecFromMat;

		for (size_t i = 0; i < Derived::size; i++) {
			for (size_t j = 0; j < Derived::size; j++) {
				vecFromMat.data[j] = self().data[j][i];
			}
			result += vecFromMat * vec.data[i];
		}

		return result;
	}

	Derived operator*(const Derived& otherMat) const {
		Derived result;
		Vec<typename Derived::value_type, Derived::size> vecFromMat;
	
		for (size_t i = 0; i < Derived::size; i++) {
			for (size_t j = 0; j < Derived::size; j++) {
				vecFromMat.data[j] = otherMat.data[j][i];
			}
	
			Vec<typename Derived::value_type, Derived::size> vec = self() * vecFromMat;
	
			for (size_t j = 0; j < Derived::size; j++) {
				result.data[j][i] = vec.data[j];
			}
		}
	
		return result;
	}

	Derived operator*=(const Derived& otherMat) {
		return self() = self() * otherMat;
	}

	void print() {
		for (size_t i = 0; i < Derived::size; i++) {
			for (size_t j = 0; j < Derived::size; j++) {
				std::cout << self().data[i][j] << " ";
			}
			std::cout << std::endl;
		}
	}
};

template<typename T>
class Mat<T, 2> : public MatMath<Mat<T, 2>>{
public:
	T data[2][2];

	using value_type = T;
	static constexpr size_t size = 2;

public:
	Mat() : data{{T{1}, T{0}}, {T{0}, T{1}}} {}
};

template<typename T>
class Mat<T, 3> : public MatMath<Mat<T, 3>>{
public:
	T data[3][3];

	using value_type = T;
	static constexpr size_t size = 3;

public:
	Mat() : data{{T{1}, T{0}, T{0}}, {T{0}, T{1}, T{0}}, {T{0}, T{0}, T{1}}} {}
};

template<typename T>
class Mat<T, 4> : public MatMath<Mat<T, 4>>{
public:
	T data[4][4];

	using value_type = T;
	static constexpr size_t size = 4;

public:
	Mat() : data{{T{1}, T{0}, T{0}, T{0}}, {T{0}, T{1}, T{0}, T{0}}, {T{0}, T{0}, T{1}, T{0}}, {T{0}, T{0}, T{0}, T{1}}} {}

	Mat(float aspect, float fov, float near, float far) : data{{1 / (tan(toRad(fov/2)) * aspect), 0, 0, 0}, {0, 1 / tan(toRad(fov/2)), 0, 0}, {0, 0, far / (near - far), far * near / (near - far)}, {0, 0, -1, 0}} {}

	void translate(const Vec3& vec) {
		data[0][3] += vec.x;
		data[1][3] += vec.y;
		data[2][3] += vec.z;
	}

	void scale(const Vec3& vec) {
		data[0][0] *= vec.x;
		data[1][1] *= vec.y;
		data[2][2] *= vec.z;
	}

	void rotateX(const float degrees) {
		float rad = toRad(degrees);
		
		Mat4 mat;

		mat.data[1][1] = cos(rad);
		mat.data[2][1] = sin(rad);
		mat.data[1][2] = -sin(rad);
		mat.data[2][2] = cos(rad);

		*this *= mat;
	}

	void rotateY(const float degrees) {
		float rad = toRad(degrees);

		Mat4 mat;

		mat.data[0][0] = cos(rad);
		mat.data[2][0] = -sin(rad);
		mat.data[0][2] = sin(rad);
		mat.data[2][2] = cos(rad);

		*this *= mat;
	}

	void rotateZ(const float degrees) {
		float rad = toRad(degrees);

		Mat4 mat;
		
		mat.data[0][0] = cos(rad);
		mat.data[1][0] = sin(rad);
		mat.data[0][1] = -sin(rad);
		mat.data[1][1] = cos(rad);

		*this *= mat;
	}
};

constexpr float toRad(float degrees) {
	return degrees * (PI / 180);
}

constexpr float toDeg(float radians) {
	return radians / (PI / 180);
}