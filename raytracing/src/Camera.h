#pragma once
#ifndef CAMERA_H
#define CAMERA_H
#include <iostream>
#include "Hittable.h"
#include "Interval.h"
#include "Ray.h"

class Camera
{
public:
    /* Public Camera Parameters Here */
    double aspect_ratio = 1.0;  // Ratio of image width over height
    int    image_width = 100;  // Rendered image width in pixel count
    int    samples_per_pixel = 10;   // Count of random samples for each pixel

    void render(const Hittable& world) {
        using namespace std;
        _initialize();
        //PPM��ʽ���
        cout << "P3\n" << image_width << ' ' << _image_height << "\n255\n";

        for (int j = 0; j < _image_height; ++j) {
            clog << "\rScanlines remaining: " << (_image_height - j) << ' ' << flush;
            for (int i = 0; i < image_width; ++i) {
                Color pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; ++sample) {
                    Ray r = _get_ray(i, j); //������ͷ����,ָ��"Ļ��"�ϼ�����ɫ����������
                    pixel_color += _ray_color(r, world); //�ۼ�,֮���� _write_color ����ƽ��
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
    void _initialize() {
        _image_height = static_cast<int>(image_width / aspect_ratio);
        _image_height = (_image_height < 1) ? 1 : _image_height;

        _center = Point3(0, 0, 0); //����ͷ����

        // Determine viewport dimensions.
        auto focal_length = 1.0;//����ͷ���ĵ�Ļ���ľ���,�ֽн���
        auto viewport_height = 2.0; //ʵ�ʳ���ӳ�䵽viewport_height viewport_width
        auto viewport_width = viewport_height * (static_cast<double>(image_width) / _image_height); //ʹ��w/h���¼���aspect_ratio,����׼

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        auto viewport_u = Vec3(viewport_width, 0, 0);       // +------�� u
        auto viewport_v = Vec3(0, -viewport_height, 0);     // |
                                                            // ��v
        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        _pixel_delta_u = viewport_u / image_width;
        _pixel_delta_v = viewport_v / _image_height;

        // Calculate the location of the upper left pixel.
        Vec3 viewport_upper_left = _center - Vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
        _pixel00_loc = viewport_upper_left + 0.5 * (_pixel_delta_u + _pixel_delta_v);
    }
    
    Ray _get_ray(int i, int j) const {
        // Get a randomly sampled camera ray for the pixel at location i,j.

        Point3 pixel_center = _pixel00_loc + (i * _pixel_delta_u) + (j * _pixel_delta_v);
        Point3 pixel_sample = pixel_center + _pixel_sample_square();

        Point3 ray_origin = _center;
        auto ray_direction = pixel_sample - ray_origin;

        return Ray(ray_origin, ray_direction);
    }
    //���������������ļ�ľ���Ϊ1����λ,һ�����صķ�Χ�Ǹ��߳�Ϊ1����λ��������
    Vec3 _pixel_sample_square() const {
        // Returns a random point in the square surrounding a pixel at the origin.
        auto px =  random_double(-0.5,0.5);
        auto py = random_double(-0.5,0.5);
        return (px * _pixel_delta_u) + (py * _pixel_delta_v);
    }

    Color _ray_color(const Ray& r, const Hittable& world) const {
        HitRecord rec;
        //������������淨�߱仯����ɫ
        if (world.hit(r, Interval(0, infinity), rec)) {
            Vec3 direction = random_on_hemisphere(rec.normal); //��ȡ�����䷽��
            Ray nextRay(rec.point, direction);
            return 0.5 * _ray_color(nextRay, world); //����
        }

        //���ر���ɫ
        Vec3 unit_direction = r.direction().normalized();
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
    }

    void _write_color(std::ostream& out, Color pixel_color, int samples_per_pixel) {
        auto r = pixel_color.x();
        auto g = pixel_color.y();
        auto b = pixel_color.z();

        // Divide the color by the number of samples.
        auto scale = 1.0 / samples_per_pixel; //��ɫsamples���ۼӵ�,֮ǰ��û��ƽ��,�ڴ˴�ƽ��
        r *= scale;
        g *= scale;
        b *= scale;

        // Write the translated [0,255] value of each color component.
        static const Interval intensity(0.000, 0.999);
        out << static_cast<int>(256 * intensity.clamp(r)) << ' '
            << static_cast<int>(256 * intensity.clamp(g)) << ' '
            << static_cast<int>(256 * intensity.clamp(b)) << '\n';
    }

};
#endif // CAMERA_H
