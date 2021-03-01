//
//  triangle.h
//  LAB
//
//  Created by 최종원 on 2021/02/01.
//

#ifndef triangle_h
#define triangle_h

#include "hittable.h"
#include "vec3.h"

class triangle : public hittable {
    public:
        triangle() {}

        triangle(point3 p1, point3 p2, point3 p3, vec3 n1, vec3 n2, vec3 n3, shared_ptr<material> m)
            : v0(p1), v1(p2), v2(p3), vn0(n1), vn1(n2), vn2(n3), mat_ptr(m) {};
        virtual bool hit(
            const ray& r, double t_min, double t_max, hit_record& rec) const override;

    public:
        point3 v0;
        point3 v1;
        point3 v2;

        vec3 vn0;
        vec3 vn1;
        vec3 vn2;

        shared_ptr<material> mat_ptr;
};

bool triangle::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 v0v1 = v1 - v0;
    vec3 v0v2 = v2 - v0;
    vec3 pvec = cross(r.dir, v0v2);

    float det = dot(v0v1, pvec);

    if(fabs(det) < 0.000001) return false;

    float invDet = 1 / det;

    vec3 tvec = r.orig - v0;
    float u = dot(tvec, pvec) * invDet;
    if(u < 0 || u > 1) return false;

    vec3 qvec = cross(tvec,v0v1);
    auto v = dot(r.dir, qvec) * invDet;
    if(v < 0 || v + u > 1) return false;

    auto t = dot(v0v2,qvec) * invDet;
    if (t < t_min || t_max < t) {
        return false;
    }

    rec.p = (u * v1) + (v * v2) + ((1 - u - v) * v0);
    rec.t = t;
    
    vec3 vertexNormal = (u * vn1) + (v * vn2) + ((1 - u - v) * vn0);
    vec3 outward_normal = unit_vector(vertexNormal);

//    vec3 outward_normal = cross(v0v1, v0v2);
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;

    return true;
}

constexpr float kEpsilon = 1e-8;




#endif /* triangle_h */
