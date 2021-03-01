//
//  sphere.h
//  LAB
//
//  Created by 최종원 on 2021/01/22.
//

#ifndef sphere_h
#define sphere_h

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
    public:
        sphere() {}
        sphere(point3 cen, double r, shared_ptr<material> m)
            : center(cen), radius(r), mat_ptr(m) {};

        virtual bool hit(
            const ray& r, double t_min, double t_max, hit_record& rec) const override;

    public:
        point3 center;
        double radius;
        shared_ptr<material> mat_ptr;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius*radius;

    auto discriminant = half_b*half_b - a*c;
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    rec.t = root; // t_min~t_max 거리 안에서 원점에서 구와 만날때 가장 짧은 벡터의 길이
    rec.p = r.at(rec.t); // t_min~t_max 거리 안에서 원점에서 구와 만날 때 원점으로 부터 거리가 가장 가까운 점
    //rec.normal = (rec.p - center) / radius; // 구의 중심과 rec.p의 방향을 가진 단위 벡터
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;
    
    return true;
}

#endif /* sphere_h */
