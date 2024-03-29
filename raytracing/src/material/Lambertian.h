#pragma once
#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H
#include "Material.h"

class Lambertian : public Material
{
private:
    Color _albedo;
public:
    Lambertian(const Color& albedo) : _albedo(albedo) {}

    //lambertian ����ɢ��ģ��

    bool scatter(const Ray& r_in, const HitRecord& hitRecord, Color& attenuation, Ray& scattered) const override {
        Vec3 scatter_direction = hitRecord.normal + random_unit_vector();

        //�������,����ķ���ǡ���� -rec.normal���޽ӽ�,ʹ��scatter_direction����������,��Ҫ���⴦��
        // Catch degenerate scatter direction
        if (vec_near_zero(scatter_direction)) {
            scatter_direction = hitRecord.normal;
        }
        scattered = Ray(hitRecord.point, scatter_direction);
        attenuation = _albedo;
        return true;
    }


};
#endif // LAMBERTIAN_H
