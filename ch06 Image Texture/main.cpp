//*************************************
// book2(RT in 1 week) ch6 Image Texture Mapping
// 相较于原书 添加如下功能：
// *(ch01&02) 修复了结果中大球体不会反射运动模糊的 bug, in material.h
// *(ch03) 修改名称 hitbale->hittable  原书book1应该是拼错了 第二版中已经修改为hittable
// *(ch03) 原书中没有给出main.cpp的修改内容，见random_scene()方法的最后一行
// *(ch03) 添加统计程序运行时间功能 使用bvh前:370s  使用后:117s
// *(ch04) 修改了贴图的颜色为 HuaweiP30Pro赤茶橘同款橘色
//*************************************

// stb图像库 from https://github.com/nothings/stb.git
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <time.h>

#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "moving_sphere.h"
#include "bvh.h"
#include "texture.h"

vec3 color(const ray& r, hittable *world, int depth) {
	hit_record rec;
	if (world->hit(r, 0.001, FLT_MAX, rec)) {
		ray scattered;
		vec3 attenuation;
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return attenuation * color(scattered, world, depth + 1);
		}
		else {
			return vec3(0, 0, 0);
		}
	} 
	else {
		// 否则，计算背景色并返回
		vec3 unit_direction = unit_vector(r.direction());
		float t = (0.5 * unit_direction.y() + 1.0);
		return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
	}
}

//scene perlin noise
hittable* two_perlin_spheres() {
	texture *pertext = new noise_texture(4);
	hittable **list = new hittable*[2];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
	list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
	//return new hittable_list(list, 2);
	return new bvh_node(list, 2, 0.0f, 1.0f);
}

// scene solid textrue
hittable* two_spheres() {
	texture *checker_1 = new checker_texture(
		new constant_texture(vec3(1.0f, 0.31f, 0.0f)),
		new constant_texture(vec3(0.9, 0.9, 0.9))
	);
	texture *checker_2 = new checker_texture(
		new constant_texture(vec3(0.0f, 1.0f, 0.8f)),
		new constant_texture(vec3(0.9, 0.9, 0.9))
	);
	int n = 50;
	hittable **list = new hittable*[n + 1];
	list[0] = new sphere(vec3(0, -10, 0), 10, new lambertian(checker_1));
	list[1] = new sphere(vec3(0, 10, 0), 10, new lambertian(checker_2));
	return new bvh_node(list, 2, 0.0f, 1.0f);
}

// scene solid textrue & image texture
hittable* random_scene()
{
	int n = 500;
	hittable **list = new hittable *[n + 1];
	texture *checker = new checker_texture(
		new constant_texture(vec3(1.0f, 0.31f, 0.0f)),
		new constant_texture(vec3(0.9f, 0.9f, 0.9f))
	);
	list[0] = new sphere(vec3(0.0f, -1000.0f, 0.0f), 1000.0f, new lambertian(checker));
	int i = 1;
	for (int a = -11; a < 11; a++)
	{
		for (int b = -11; b < 11; b++)
		{
			float choose_mat = (rand() % 100 / float(100));
			vec3 center(a + 0.9f*(rand() % 100 / float(100)), 0.2f, b + 0.9f*(rand() % 100 / float(100)));
			if ((center - vec3(4.0f, 0.2f, 0.0f)).length() > 0.9f)
			{
				if (choose_mat < 0.8f)
				{
					list[i++] = new moving_sphere(center, center+vec3(0, (rand() % 100 / float(100))*0.5, 0), 0.0, 1.0, 0.2f,
						new lambertian(new constant_texture(vec3((rand() % 100 / float(100)) * (rand() % 100 / float(100)), (rand() % 100 / float(100)) * (rand() % 100 / float(100)), (rand() % 100 / float(100)) * (rand() % 100 / float(100))))));
				}
				else if (choose_mat < 0.95f)
				{
					list[i++] = new sphere(center, 0.2f,
						new metal(vec3(0.5f * (1 + (rand() % 100 / float(100))), 0.5f * (1 + (rand() % 100 / float(100))), 0.5f * (1 + (rand() % 100 / float(100)))), 0.5f * (rand() % 100 / float(100))));
				}
				else
				{
					list[i++] = new sphere(center, 0.2f, new dielectric(1.5f));
				}
			}
		}
	}

	// image texture
	int nx, ny, nn;
	unsigned char *tex_data = stbi_load("earthmap.jpg", &nx, &ny, &nn, 0);

	list[i++] = new sphere(vec3(0.0f, 1.0f, 0.0f), 1.0f, new dielectric(1.5f));
	list[i++] = new sphere(vec3(-4.0f, 1.0f, 0.0f), 1.0f, new metal(vec3(0.7f, 0.6f, 0.5f), 0.0f));
	list[i++] = new sphere(vec3(4.0f, 1.0f, 0.0f), 1.0f, new lambertian(new image_texture(tex_data, nx, ny)));
	//return new hittable_list(list, i);
	return new bvh_node(list, i, 0.0f, 1.0f); //使用bvh
}


int main() {
	// 统计程序运行时间
	clock_t start_time, end_time;
	start_time = clock();
	
	// width, height, channels of image
	int nx = 2000; // width
	int ny = 1000; // height
	int channels = 3;
	int ns = 32; // sample count

	// 存储图像数据
	unsigned char *data = new unsigned char[nx*ny*channels];
	
	// 场景相关
	hittable *world = random_scene();
	// camera
	vec3 lookfrom(13, 2, 3);
	vec3 lookat(0, 0, 0);
	float dist_to_focus = 10.0;
	float aperture = 0.0;
	camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);

	// 循环遍历图像nx*ny中的每个像素
	for (int j = ny - 1; j >= 0; j--) {
		for (int i = 0; i < nx; i++) {
			vec3 col(0,0,0);
			// 采样 ns 次
			for (int k = 0; k < ns; k++) {
				float u = float(i + (rand() % 100 / float(100))) / float(nx);
				float v = float(j + (rand() % 100 / float(100))) / float(ny);
				// 确定 ray r
				ray r = cam.get_ray(u, v);
				// 累加 ray r 射入场景 world 后，返回的颜色
				col += color(r, world, 0);
			}
			col /= float(ns);
			// gammar 矫正
			col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
			// 写入图像数据 data[y*width*channels + x*channels + index]
			data[(ny - j - 1)*nx*3 + 3 * i + 0] = int(255.99*col[0]);
			data[(ny - j - 1)*nx*3 + 3 * i + 1] = int(255.99*col[1]);
			data[(ny - j - 1)*nx*3 + 3 * i + 2] = int(255.99*col[2]);
		}
		// print渲染进度
		std::cout << (ny - j) / float(ny) * 100.0f << "%\n";
	}
	end_time = clock();
	std::cout << "Total time : " << (double)(end_time - start_time) / CLOCKS_PER_SEC << "s" << std::endl;
	// 写出png图片
	stbi_write_png("book2-ch06.png", nx, ny, channels, data, 0);

	std::cout << "Completed.\n";
	system("PAUSE");
	return 0;
}