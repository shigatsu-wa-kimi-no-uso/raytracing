#pragma once
#ifndef HITTABLE_H
#define HITTABLE_H
#include <memory>
#include <vector>
#include "Ray.h"
#include "Interval.h"

using std::shared_ptr;
using std::make_shared;

struct HitRecord 
{
    Point3 point;
    Vec3 normal;
    double t;
    bool front_face;
};

class Hittable
{
public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const = 0;

    virtual void set_face_normal(const Ray& r, const Vec3& outward_normal, HitRecord& rec) const {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outward_normal` is assumed to have unit length.
        //���ݼ���������ķ������͹��߷���������������жϹ��������˼����������(����)�����ڱ���
        rec.front_face = r.direction().dot(outward_normal) < 0;
        rec.normal = rec.front_face ? outward_normal : -outward_normal;
    }
};

class HittableList : public Hittable {
public:
    std::vector<shared_ptr<Hittable>> objects;

    HittableList() {}
    HittableList(shared_ptr<Hittable> object) { add(object); }

    void clear() { objects.clear(); }

    void add(shared_ptr<Hittable> object) {
        objects.push_back(object);
    }

    bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
        HitRecord temp_rec;
        bool hit_anything = false;
        double closest_so_far = ray_t.max;
        //���������б�,����ж�ÿ�������Ƿ�������ཻ
        //Interval(ray_t.min, closest_so_far): ���ƹ��ߴ����ĳ���,���ߴ�����ͷ���򳡾�,ͨ��һ��"����"ֻ��"����"���������(t��С),
        //��Զ�����屻�������嵲ס,ͨ��interval�ɺ��Ը����ֱ���ж�Ϊ"δ����"
        for (const shared_ptr<Hittable>& object : objects) {
            if (object->hit(r, Interval(ray_t.min, closest_so_far), temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }
};


#endif // HITTABLE_H