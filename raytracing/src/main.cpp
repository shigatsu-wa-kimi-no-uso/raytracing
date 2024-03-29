#include "global.h"
#include "hittable/Hittable.h"
#include "hittable/Sphere.h"
#include "material/Metal.h"
#include "material/Lambertian.h"
#include "material/Dielectric.h"
#include "Camera.h"


int main() {
    HittableList world;

    auto material_ground = make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
    auto material_center = make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
    auto material_left   = make_shared<Dielectric>(1.5);
    auto material_right  = make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.0);

    world.add(make_shared<Sphere>(Point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<Sphere>(Point3( 0.0,    0.0, -1.0),   0.5, material_center));
    world.add(make_shared<Sphere>(Point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(make_shared<Sphere>(Point3(-1.0,    0.0, -1.0),  -0.4, material_left));
    world.add(make_shared<Sphere>(Point3( 1.0,    0.0, -1.0),   0.5, material_right));

    Camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 10;

    cam.fov     = 90;
    cam.lookfrom = Point3(0, 0, 0);
    cam.lookat   = Point3(0,0,-1);
    cam.vup      = Vec3(0,1,0);

    cam.defocus_angle = 10.0;
    cam.focus_dist = 1.4;

    cam.render(world);
}