#pragma once
#ifndef METAL_H
#define METAL_H
#include "Material.h"


class Metal : public Material {
public:
    Metal(const Color& albedo, double fuzz) : _albedo(albedo), _fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const Ray& r_in, const HitRecord& hitRecord, Color& attenuation, Ray& scattered) const override {
        Vec3 reflected = reflect(r_in.direction().normalized(), hitRecord.normal);
        scattered = Ray(hitRecord.point, reflected + _fuzz * random_unit_vector());
        attenuation = _albedo;
        return scattered.direction().dot( hitRecord.normal) > 0; //С��0���: �������fuzz���˵�ǰ��ı���,ֱ�Ӱ����߱����մ���,����false
    }

private:
    Color _albedo;
    double _fuzz;
};

#endif // METAL_H
