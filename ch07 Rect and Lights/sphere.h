#pragma once
#include "hittable.h"
#define _USE_MATH_DEFINES
#include <math.h>
class sphere :public hittable {
public:
	sphere() {}
	sphere(vec3 cen, float r, material *m) { center = cen; radius = r; mat_ptr = m; }
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
	//bool bounding_box(float t0, float t1, aabb & box) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const;
	void get_sphere_uv(const vec3& p, float& u, float &v) const;
	vec3 center;
	float radius;
	material *mat_ptr;
};

void sphere::get_sphere_uv(const vec3& p, float& u, float &v) const {
	float phi = atan2(p.z(), p.x());
	float theta = asin(p.y());
	u = 1 - (phi + M_PI) / (2 * M_PI);
	v = (theta + M_PI / 2) / M_PI;
}

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
	vec3 A_C = r.origin() - center;
	vec3 B = r.direction();
	float a = dot(B, B);
	float b = dot(A_C, B);
	float c = dot(A_C, A_C) - radius * radius;
	float discriminant = b * b - a * c;
	if (discriminant > 0) {
		float temp = (-b - sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center) / radius;
			rec.mat_ptr = mat_ptr;
			get_sphere_uv((rec.p - center) / radius, rec.u, rec.v);
			return true;
		}
		temp = (-b + sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center) / radius;
			rec.mat_ptr = mat_ptr;
			get_sphere_uv((rec.p - center) / radius, rec.u, rec.v);
			return true;
		}
	}
	return false;
}

bool sphere::bounding_box(float t0, float t1, aabb& box) const {
	box = aabb(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
	return true;
}