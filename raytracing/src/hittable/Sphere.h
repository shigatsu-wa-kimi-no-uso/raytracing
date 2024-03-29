#pragma once
#ifndef SPHERE_H
#define SPHERE_H
#include "Hittable.h"

class Sphere : public Hittable
{
private:
    Point3 _center;
    double _radius;
    shared_ptr<Material> _material;
public:

    Sphere(Point3 center, double radius, shared_ptr<Material> material) 
        : _center(center), _radius(radius), _material(material){}
    //Sphere: (P-C).*(P-C) = r^2
    //Ray: P(t) = A-t*b   A为光线发射点(取camera的中心),b为光线的方向
    //联立求解: (A-t*b).*(A-t*b)=r^2 =>  (b.*b)*t^2 + 2*(b.*(A-C))*t + (A-C).*(A-C)-r^2=0
    //应用求根公式即可求解                ^^^^^^a      ^^^^^^^^^^^^b    ^^^^^^^^^^^^^^^^c
    bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
        //[-b + sqrt(b^2-4*a*c)]/2*a =={b=2h}== [-h + sqrt(h^2-a*c)]/a
        
        Vec3 oc = r.origin() - _center; // A-C
        double a = r.direction().squaredNorm(); //norm:长度(模) squaredNorm: 长度的平方
        double half_b = oc.dot(r.direction());
        double c = oc.squaredNorm() - _radius * _radius;

        double discriminant = half_b * half_b - a * c;
        if (discriminant < 0) return false;
        double sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        //在取值范围内找值最小的根
        double root = (-half_b - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (-half_b + sqrtd) / a;
            if (!ray_t.surrounds(root)) {
                return false;
            }
        }

        rec.t = root;
        rec.point = r.at(rec.t);
        rec.material = _material;
        Vec3 outward_normal = (rec.point - _center) / _radius;
        set_face_normal(r, outward_normal, rec);

        return true;
    }

};

#endif // SPHERE_H