#pragma once
#ifndef CAMERA_H
#define CAMERA_H
#include <iostream>
#include "hittable/Hittable.h"
#include "global.h"
#include "Interval.h"
#include "Ray.h"

class Camera
{
public:
    /* Public Camera Parameters Here */
    double aspect_ratio = 1.0;  // Ratio of image width over height
    int    image_width = 100;  // Rendered image width in pixel count
    int    samples_per_pixel = 10;   // Count of random samples for each pixel
    int    max_depth = 10;   // Maximum number of ray bounces into scene

    double fov = 50;  // Vertical view angle (field of view) (degree)
    Point3 lookfrom = Point3(0, 0, -1);  // Point camera is looking from
    Point3 lookat = Point3(0, 0, 0);   // Point camera is looking at
    Vec3   vup = Vec3(0, 1, 0);     // Camera-relative "up" direction

    double defocus_angle = 0;  // Variation angle of rays through each pixel
    double focus_dist = 10;    // Distance from camera lookfrom point to plane of perfect focus

    void render(const Hittable& world) {
        using namespace std;
        _initialize();
        //PPM格式输出
        cout << "P3\n" << image_width << ' ' << _image_height << "\n255\n";

        for (int j = 0; j < _image_height; ++j) {
            clog << "\rScanlines remaining: " << (_image_height - j) << ' ' << flush;
            for (int i = 0; i < image_width; ++i) {
                Color pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; ++sample) {
                    Ray r = _get_ray(i, j); //从摄像头中心,指向"幕布"上即将上色的像素中心
                    pixel_color += _ray_color(r, max_depth,world); //累加,之后在 _write_color 中求平均
                }
                _write_color(cout, pixel_color, samples_per_pixel);
            }
        }

        clog << "\rDone.                 \n";
    }

private:
    /* Private Camera Variables Here */
    int    _image_height;   // Rendered image height
    Point3 _center;         // Camera center
    Point3 _pixel00_loc;    // Location of pixel 0, 0
    Vec3   _pixel_delta_u;  // Offset to pixel to the right
    Vec3   _pixel_delta_v;  // Offset to pixel below
    Vec3   _u, _v, _w;        // Camera frame basis vectors
    Vec3   _defocus_disk_u;  // Defocus disk horizontal radius
    Vec3   _defocus_disk_v;  // Defocus disk vertical radius

    void _initialize() {
        _image_height = static_cast<int>(image_width / aspect_ratio);
        _image_height = (_image_height < 1) ? 1 : _image_height;

        _center = lookfrom; //摄像头中心

        // Determine viewport dimensions.
       // double focal_length = (lookfrom - lookat).norm();//摄像头中心到幕布的距离,又叫焦距
        double theta = deg2rad (fov);
        double h = tan(theta / 2);
        double viewport_height = 2 * h * focus_dist; //实际长宽映射到viewport_height viewport_width
        double viewport_width = viewport_height * (static_cast<double>(image_width) / _image_height); //使用w/h重新计算aspect_ratio,更精准

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        _w = (lookfrom - lookat).normalized();
        _u = vup.cross(_w).normalized();
        _v = _w.cross(_u);
        Vec3 viewport_u = viewport_width * _u;       // +------→ u
        Vec3 viewport_v = viewport_height *(- _v);     // |
                                                            // ↓v
        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        _pixel_delta_u = viewport_u / image_width;
        _pixel_delta_v = viewport_v / _image_height;

        // Calculate the location of the upper left pixel.
        Vec3 viewport_upper_left = _center - (focus_dist * _w) - viewport_u / 2 - viewport_v / 2;
        _pixel00_loc = viewport_upper_left + 0.5 * (_pixel_delta_u + _pixel_delta_v);

        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius = focus_dist * tan(deg2rad(defocus_angle / 2));
        _defocus_disk_u = _u * defocus_radius;
        _defocus_disk_v = _v * defocus_radius;
    }
    
    Ray _get_ray(int i, int j) const {
        // Get a randomly-sampled camera ray for the pixel at location i,j, originating from
        // the camera defocus disk.
        Point3 pixel_center = _pixel00_loc + (i * _pixel_delta_u) + (j * _pixel_delta_v);
        Point3 pixel_sample = pixel_center + _pixel_sample_square();
        //要实现blur,需多次采样
        Point3 ray_origin = (defocus_angle <= 0) ? _center : _defocus_disk_sample();
        Vec3 ray_direction = pixel_sample - ray_origin;

        return Ray(ray_origin, ray_direction);
    }
    //两个相邻像素中心间的距离为1个单位,一个像素的范围是个边长为1个单位的正方形
    Vec3 _pixel_sample_square() const {
        // Returns a random point in the square surrounding a pixel at the origin.
        auto px =  random_double(-0.5,0.5);
        auto py = random_double(-0.5,0.5);
        return (px * _pixel_delta_u) + (py * _pixel_delta_v);
    }

    Point3 _defocus_disk_sample() const {
        // Returns a random point in the camera defocus disk.
        auto p = random_in_unit_disk();
        return _center + (p[0] * _defocus_disk_u) + (p[1] * _defocus_disk_v);
    }

    Color _ray_color(const Ray& r,int depth, const Hittable& world) const {
        HitRecord hitRecord;
        if (depth <= 0) {
            return Color(0, 0, 0); //达到一定深度后,返回黑色
        }

        //返回随物体表面法线变化的颜色
        if (world.hit(r, Interval(0.001, infinity), hitRecord)) { 
            //Interval(0.001, infinity): 限制解的范围,解决acne problem的一种方式
            Ray nextRay;
            Color attenuation; //衰减,由反照率albedo确定
            if (hitRecord.material->scatter(r, hitRecord, attenuation, nextRay)) {
                return attenuation.cwiseProduct(_ray_color(nextRay, depth - 1, world)); //子光线 
            } else {
                return Color(0, 0, 0);
            }

        }

        //返回背景色
        Vec3 unit_direction = r.direction().normalized();
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
    }

    void _write_color(std::ostream& out, Color pixel_color, int samples_per_pixel) {
        auto r = pixel_color.x();
        auto g = pixel_color.y();
        auto b = pixel_color.z();

        // Divide the color by the number of samples.
        auto scale = 1.0 / samples_per_pixel; //颜色samples是累加的,之前并没有平均,在此处平均
        r *= scale;
        g *= scale;
        b *= scale;

        // Apply the linear to gamma transform.
        r = linear_to_gamma(r); //gamma矫正
        g = linear_to_gamma(g);
        b = linear_to_gamma(b);

        // Write the translated [0,255] value of each color component.
        static const Interval intensity(0.000, 0.999);
        out << static_cast<int>(256 * intensity.clamp(r)) << ' '
            << static_cast<int>(256 * intensity.clamp(g)) << ' '
            << static_cast<int>(256 * intensity.clamp(b)) << '\n';
    }

};
#endif // CAMERA_H
