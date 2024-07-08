#ifndef UTILMATH_H
#define UTILMATH_H

#include <cmath>
#define EPSILON 0.0001
#define M_PI 3.14159265358979323846

namespace Eigen {
	template<typename Scalar>
	Eigen::Matrix<Scalar, 4, 4> perspective(Scalar fovy, Scalar aspect, Scalar zNear, Scalar zFar) {
		Transform<Scalar, 3, Projective> tr;
		tr.matrix().setZero();
		assert(aspect > 0);
		assert(zFar > zNear);
		assert(zNear > 0);
		Scalar radf = M_PI * fovy / 180.0;
		Scalar tan_half_fovy = std::tan(radf / 2.0);
		tr(0, 0) = 1.0 / (aspect * tan_half_fovy);
		tr(1, 1) = 1.0 / (tan_half_fovy);
		tr(2, 2) = -(zFar + zNear) / (zFar - zNear);
		tr(3, 2) = -1.0;
		tr(2, 3) = -(2.0 * zFar * zNear) / (zFar - zNear);
		return tr.matrix();
	}
}

namespace UtilMath
{

	template <typename T>
	inline bool isZero(T value) 
	{
		return std::fabs(value) < EPSILON;
	}

	template <typename T>
	inline bool isNearlyEqual(T a, T b) 
	{
		return std::fabs(a - b) < EPSILON;
	}

	// NOTE: Attention les paramètres doivent avoir le même type
	// exemple ayant un Eigen::Vector3f v, pour que le compilateur trouve un overload de isSumNearlyX
	// isSumNearlyX(v[0], v[1], v[2], 1.0f) fonctionnera tandis que isSumNearlyX(v[0], v[1], v[2], 1.0) ne fonctionnera pas
	
	template <typename T>
	bool isSumNearlyL(T sum, T a) {
		return sum < a;
	}

	template <typename T, typename... Ts> 
	bool isSumNearlyL(T a, T b, Ts... args) {
		return isSumNearlyL(a + b, args...);
	}

	template <typename T>
	bool isSumNearlyG(T sum, T a) {
		return sum > a;
	}

	template <typename T, typename... Ts>
	bool isSumNearlyG(T a, T b, Ts... args) {
		return isSumNearlyG(a + b, args...);
	}

	template <typename T>
	bool isSumNearlyLE(T sum, T a) {
		return sum <= a;
	}

	template <typename T, typename... Ts>
	bool isSumNearlyLE(T a, T b, Ts... args) {
		return isSumNearlyLE(a + b, args...);
	}

	template <typename T>
	bool isSumNearlyGE(T sum, T a) {
		return sum >= a;
	}

	template <typename T, typename... Ts>
	bool isSumNearlyGE(T a, T b, Ts... args) {
		return isSumNearlyGE(a + b, args...);
	}

	template <typename T>
	bool isSumNearlyE(T sum, T a) {
		return isNearlyEqual(sum, a);
	}

	template <typename T, typename... Ts>
	bool isSumNearlyE(T a, T b, Ts... args) {
		return isSumNearlyE(a + b, args...);
	}

}

#endif