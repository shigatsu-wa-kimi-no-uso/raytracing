#pragma once
#ifndef DIELECTRIC_H
#define DIELECTRIC_H
#include "Material.h"

class Dielectric : public Material {
public:
    Dielectric(double index_of_refraction) : _ior(index_of_refraction) {}

    bool scatter(const Ray& r_in, const HitRecord& hitRecord, Color& attenuation, Ray& scattered) const override {
        attenuation = Color(1.0, 1.0, 1.0);
        double refraction_ratio = hitRecord.front_face ? (1.0 / _ior) : _ior;
        // refraction_ratio = etai/etat    从空气(etai=1)入射到介质(etat=ior)

        Vec3 unit_direction = r_in.direction().normalized();

        double cos_theta = std::min(hitRecord.normal.dot(-unit_direction), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        Vec3 direction;


        //当可以折射时,随机选择一种情况选择是折射还是反射,否则只反射
        if (cannot_refract || _reflectance(cos_theta, refraction_ratio) > random_double(0,1)) {
            direction = reflect(unit_direction, hitRecord.normal);
        } else {
            direction = refract(unit_direction, hitRecord.normal, refraction_ratio);
        }

        scattered = Ray(hitRecord.point, direction);
        return true;
    }

private:
    double _ior; // Index of Refraction (介质相对与真空的折射率,真空eta=1,光从介质1射入介质2,介质2相对于介质1的折射率eta_21=eta_2/eta_1)

    static double _reflectance(double cosine, double ref_idx) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};

#endif
