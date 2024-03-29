#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H
#include "../global.h"
#include "../Ray.h"
#include "../hittable/Hittable.h"

struct HitRecord;

class Material
{
public:
    virtual ~Material() = default;

    virtual bool scatter(const Ray& r_in, const HitRecord& hitRecord, Color& attenuation, Ray& scattered) const = 0; 
    //���ڲ�ͬ�Ĳ���(Material������)����ȡ��ͬ��scatter������ɢ�䣬���䣬����ȣ�
};


inline Vec3 reflect(const Vec3& unit_r_in, const Vec3& normal) {
    return unit_r_in - 2 * unit_r_in.dot(normal) * normal;
}

inline Vec3 refract(const Vec3& unit_r_in, const Vec3& n, double etai_over_etat) {
    //������ߵ�λ���� unit_r_in
    double cos_theta = std::min(n.dot(-unit_r_in), 1.0);
    Vec3 r_out_perp = etai_over_etat * (unit_r_in + cos_theta * n);
    Vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.squaredNorm())) * n;
    return r_out_perp + r_out_parallel;
}


#endif // MATERIAL_H