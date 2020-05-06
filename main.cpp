#include "rtweekend.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include "camera.hpp"

#include <vector>
#include <iostream>

using namespace std;

hittable_list random_scene()
{
  hittable_list world;

  world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(color(0.5, 0.5, 0.5))));

  int i = 1;
  for (int a = -11; a < 11; a++)
  {
    for (int b = -11; b < 11; b++)
    {
      auto choose_mat = random_double();
      point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
      if ((center - vec3(4, 0.2, 0)).length() > 0.9)
      {
        if (choose_mat < 0.8)
        {
          // diffuse
          auto albedo = color::random() * color::random();
          world.add(
              make_shared<sphere>(center, 0.2, make_shared<lambertian>(albedo)));
        }
        else if (choose_mat < 0.95)
        {
          // metal
          auto albedo = color::random(.5, 1);
          auto fuzz = random_double(0, .5);
          world.add(
              make_shared<sphere>(center, 0.2, make_shared<metal>(albedo, fuzz)));
        }
        else
        {
          // glass
          world.add(make_shared<sphere>(center, 0.2, make_shared<dielectric>(1.5)));
        }
      }
    }
  }

  world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, make_shared<dielectric>(1.5)));
  world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, make_shared<lambertian>(color(.4, .2, .1))));
  world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, make_shared<metal>(color(.7, .6, .5), 0.0)));

  return world;
}

double hit_sphere(const point3 &center, double radius, const ray &r)
{
  /**
   * See section "Adding a Sphere"
   */

  vec3 oc = r.origin() - center;
  auto a = r.direction().length_squared();
  auto half_b = dot(oc, r.direction());
  auto c = oc.length_squared() - radius * radius;
  auto discriminant = half_b * half_b - a * c;

  if (discriminant < 0)
  {
    return -1.0;
  }
  else
  {
    return (-half_b - sqrt(discriminant)) / a;
  }
}

color ray_color(const ray &r, const hittable &world, int depth)
{
  hit_record rec;
  // If we've exceeded the ray bounce limit, no more light is gathered.
  if (depth <= 0)
    return color(0, 0, 0);

  if (world.hit(r, 0.001, infinity, rec))
  {
    ray scattered;
    color attenuation;
    if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
      return attenuation * ray_color(scattered, world, depth - 1);

    return color(0, 0, 0);
  }

  vec3 unit_direction = unit_vector(r.direction());
  auto t = 0.5 * (unit_direction.y() + 1.0);

  /**
   * I then did a standard graphics trick of scaling that to 0.0≤t≤1.0. When t=1.0 I want blue. When t=0.0
   * I want white. In between, I want a blend. This forms a “linear blend”, or “linear interpolation”, or “lerp” for short, between two things. 
   * A lerp is always of the form:
   * 
   * blendedValue=(1−t)⋅startValue+t⋅endValue,
*/
  return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

int main()
{
  const auto aspect_ratio = 16.0 / 9.0;
  const int image_width = 384;
  const int image_height = static_cast<int>(image_width / aspect_ratio);
  const int samples_per_pixel = 100;
  const int max_depth = 50;

  cout << "P3\n"
       << image_width << ' ' << image_height << "\n255\n";

  auto world = random_scene();

  point3 lookfrom(13, 2, 3);
  point3 lookat(0, 0, 0);
  vec3 vup(0, 1, 0);
  auto dist_to_focus = 10.0;
  auto aperture = 0.1;

  camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

  for (int j = image_height - 1; j >= 0; --j)
  {
    cerr << "\nScanlines remaining: " << j << ' ' << flush;

    for (int i = 0; i < image_width; ++i)
    {
      color pixel_color(0, 0, 0);
      for (int s = 0; s < samples_per_pixel; ++s)
      {
        auto u = (i + random_double()) / (image_width - 1);
        auto v = (j + random_double()) / (image_height - 1);
        ray r = cam.get_ray(u, v);
        pixel_color += ray_color(r, world, max_depth);
      }

      write_color(cout, pixel_color, samples_per_pixel);
    }
  }

  cerr << "\nDone.\n";

  return 0;
}
