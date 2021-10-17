#pragma once
#include "ray.h"
#include "random.h"
#include <algorithm>


//Types of materials: diffuse (perfect diffuse), reflective (perfectly reflective), glossy (diffuse n reflective), dilectric (refractive)
//add as needed
enum class material_type
{
    lambertian,
    metal,
    dielectric
};

class material {
private:
    vec3 albedo;
    double fuzz;
    double ref_idx;
    material_type mat;

    //Determines if a point is under shadow, only handles point/dir lights, TODO: improve this.
    /*
    inline bool under_shadow(const hit_record& rec, hitable* world, const vec3& light)
    {
        //vec3 l = (directional) ? -light : light - rec.p;
        if (dot(light, rec.normal) < 0)
            return true;

        hit_record shadow_rec;
        ray shadow_ray = ray(rec.p, light);
        if (world->hit(shadow_ray, 0.0001, FLT_MAX, shadow_rec))
            return true;

        return false;
    }
    */

    /**
    * Computes the Shlick approximation to the Fresnel equations, used in determining the probability an incident ray is reflected, given
    * its angle of incidence.
    * @param cosine - the angle between the incident vector and surface normal.
    * @param ref_idx - index of refraction of incident medium or its inverse (used when idx in incident medium > idx of air, i.e. object
    *                  is hitting surface from the inside).
    * @return the approximate probability the incident ray is reflected.
    */
    inline double schlick(double cosine, double ref_idx) {
        double r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1.0f - r0) * pow((1 - cosine), 5);
    }


    /**
    * Gets the transmitted vector through this material, if there is one, using standard geometric refraction calculation.
    * @param v - the direction of the incident ray.
    * @param n - the surface normal at point of intersection.
    * @param nr - relative indices of refraction, numerator is index of medium in which incident ray is traveling.
    * @return true if the incident ray is transmitted, false otherwise.
    */
    inline bool refract(const vec3& v, const vec3& n, double nr, vec3& refracted) {
        vec3 uv = unit_vector(v);
        double dt = dot(uv, n);
        double discriminant = 1.0f - nr * nr * (1.0f - dt * dt);
        if (discriminant > 0) {
            refracted = nr * (uv - n * dt) - n * sqrt(discriminant);
            return true;
        }
        else
            return false;
    }

    /**
    * Gets a random point within the unit sphere.
    */
    inline vec3 random_point()
    {
        vec3 p;
        do
        {
            p = 2.0 * vec3(random_double(), random_double(), random_double()) - vec3(1, 1, 1);
        } while (p.squared_length() >= 1.0);

        return p;
    }

    /**
    * Reflects the specified vector symmetrically about the specified normal.
    */
    inline vec3 reflect(const vec3& v, const vec3& n) {
        return v - 2 * dot(v, n) * n;
    }

public:
    
    //Default
    material() {}

    //For Lambertian materials
    material(vec3 a, material_type m) : albedo{ a }, mat { m } {  }

    /**
    * Constructs a metal material.
    * @param a - the albedo, i.e the degree of reflection (1 for 100%).
    * @param f - the fuziness factor, used to control spread of scattering, with 0 there is no perterbation from direction of reflection.
    **/
    material(vec3 a, material_type m, double f) : albedo{ a }, mat{ m }
    {
        fuzz = (f < 1) ? f : 1;
    }

    //For dielectrics
    material(material_type m, double ri) : mat{ m }, ref_idx{ ri } {}

    /**
    * The function responsible for determining how incident rays interact with this material, will appropriately deduce if an incident ray is
    * absorbed, reflected, or transmitted. In doing so it will attenuate incident rays, and for those unabsorbed, update their direction as well.
    * (attenuate = reduce value of, so absorbed rays are in essence fully attenuated)
    * @param r_in - the ray incident to the surface of the object described by this material.
    * @param rec - the hit record of the ray.
    * @param attenuation - by how much are incident colour_channels unabsorbed (specified by material properties, a 1 = perfect reflection)
    *                      Val to be computed, this argument is a holder for function caller to have access to attenuation upon return.
    * @param scattered - holds the ray that will be scattered from the point of intersection (generic term for reflected/transmitted)
    * @return true if the incident ray is scattered, false if it is otherwise absorbed.
    */
    bool scatter(const ray& r, const hit_record& rec, vec3& attenuation, ray& scattered);

};

bool material::scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered)
{
    bool ret;

    vec3 target;
    vec3 reflected;

    vec3 outward_normal; 
    double ni_over_nt;
    vec3 refracted;
    double reflect_prob;
    double cosine, theta;

    switch (mat)
    {
    case material_type::lambertian:
        target = rec.p + rec.normal + random_point();
        attenuation = albedo;
        scattered = ray(rec.p, target - rec.p);
        ret = true;
        break;
    case material_type::metal:
        reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        attenuation = albedo;
        scattered = ray(rec.p, reflected + fuzz * random_point());
        ret = (dot(scattered.direction(), rec.normal) > 0);
        break;
    case material_type::dielectric:
        reflected = reflect(r_in.direction(), rec.normal);
        attenuation = vec3(1.0, 1.0, 1.0);
        if (dot(r_in.direction(), rec.normal) > 0) {
            outward_normal = -rec.normal;
            ni_over_nt = ref_idx;
            // cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();
            theta = dot(r_in.direction(), rec.normal) / r_in.direction().length();
            cosine = sqrt(1 - ref_idx * ref_idx * (1 - theta * theta));
        }
        else {
            outward_normal = rec.normal;
            ni_over_nt = 1.0 / ref_idx;
            cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }
        if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
            reflect_prob = schlick(cosine, ref_idx);
        else
            reflect_prob = 1.0;
        if (random_double() < reflect_prob)
        {
            scattered = ray(rec.p, reflected);
        }
        else
        {
            scattered = ray(rec.p, refracted);
        }
        ret = true;
        break;
    default:
        ret = false;
        break;
    }

    return ret;
}