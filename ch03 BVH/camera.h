#pragma once
#define _USE_MATH_DEFINES
#include "ray.h"
#include <math.h>

vec3 random_in_unit_disk() {
	vec3 p;
	do {
		p = 2.0*vec3((rand() % 100 / float(100)), (rand() % 100 / float(100)), 0) - vec3(1, 1, 0);
	} while (dot(p, p) >= 1.0);
	return p;
}

class camera {
public:
	// 新增：t0 t1
	camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist, float t0, float t1) {
		lens_radius = aperture / 2;
		time0 = t0; // new
		time1 = t1; // new
		float theta = vfov *  M_PI / 180;
		float half_height = tan(theta / 2);
		float half_width = aspect * half_height;

		origin = lookfrom;
		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);

		lower_left_corner = origin - half_width*focus_dist*u - half_height*focus_dist*v - focus_dist*w; // 左下角
		horizontal = 2*half_width*focus_dist*u; // 横轴
		vertical = 2*half_height*focus_dist*v; // 竖轴
	}
	ray get_ray(float s, float t) { 
		vec3 rd = lens_radius * random_in_unit_disk();
		vec3 offset = u * rd.x() + v * rd.y();
		float time = time0 + (rand() % 100 / float(100))*(time1 - time0);
		return ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset, time);
	}

	vec3 lower_left_corner;
	vec3 origin;
	vec3 horizontal;
	vec3 vertical;
	vec3 u, v, w;
	float time0, time1; // 新增 time0 time1
	float lens_radius;
};