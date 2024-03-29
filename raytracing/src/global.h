#pragma once
#ifndef GLOBAL_H
#define GLOBAL_H
#include <eigen3/Eigen/Eigen>
#include <chrono>
#include <random>
using Point3 = Eigen::Vector3d;
using Vec3 = Eigen::Vector3d;
using Color = Eigen::Vector3d;
using std::shared_ptr;
using std::make_shared;
using std::sqrt;


const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

inline double deg2rad(double deg) {
    return deg * pi / 180.0;
}

inline time_t current_time_nanos() {
    using namespace std;
    chrono::system_clock::time_point now = chrono::system_clock::now();
    chrono::nanoseconds nanoseconds = chrono::duration_cast<chrono::nanoseconds>(now.time_since_epoch());
    return nanoseconds.count();
}

inline double random_double() {
    // Returns a random real in [0,1).
    using namespace std;
    static default_random_engine dre((uint32_t)current_time_nanos());
    static uniform_real_distribution<double> uniform(0, 1);
    return uniform(dre);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}


inline Vec3 random_unit_vector() {
    return Vec3::Random().normalized();
}

inline Vec3 random_in_unit_disk() {
    while (true) {
        auto p = Vec3(random_double(-1.0, 1.0), random_double(-1.0, 1.0), 0);
        if (p.squaredNorm() < 1)
            return p;
    }
}


inline Vec3 random_on_hemisphere(const Vec3& normal) {
    Vec3 on_unit_sphere = random_unit_vector();
    if (on_unit_sphere.dot(normal) > 0.0) { // In the same hemisphere as the normal
        return on_unit_sphere;
    } else {
        return -on_unit_sphere;
    }
}


inline double linear_to_gamma(double linear_component)
{
    return sqrt(linear_component);
}

inline bool vec_near_zero(const Vec3& v) {
    // Return true if the vector is close to zero in all dimensions.
    double s = 1e-8;
    return (fabs(v(0)) < s) && (fabs(v(1)) < s) && (fabs(v(2)) < s);
}

#endif // GLOBAL_H
