#include <catch2/catch_test_macros.hpp>
#include "Geometry.h"
#include "CollisionTester.h"

TEST_CASE("Geometry Test case")
{
	REQUIRE(1 == 1);
}

TEST_CASE("Sphere Intersections")
{
    CollisionTester intersectionTester;

    Sphere sphere(vector(0, 0, 0), 2);
    Sphere anotherSphere(vector(3, 1, 1), 2);
    Plane plane(vector(0, 0, 0), vector(0, 1, 0));

    AABB aabb(vector(-1, 0, 0), vector(1, 1, 1));

    REQUIRE(intersectionTester.intersects((Geometry &)sphere, (Geometry &)anotherSphere));
    REQUIRE(intersectionTester.intersects((Geometry &)sphere, (Geometry &)plane));
    REQUIRE(intersectionTester.intersects((Geometry &)sphere, (Geometry &)aabb));

    anotherSphere.setOrigin(vector(0, 6, 0));
    REQUIRE(!intersectionTester.intersects((Geometry &)sphere, (Geometry &)anotherSphere));
    REQUIRE(!intersectionTester.intersects((Geometry &)anotherSphere, (Geometry &)plane));
    REQUIRE(!intersectionTester.intersects((Geometry &)anotherSphere, (Geometry &)aabb));
}

TEST_CASE("Plane Intersections")
{
    CollisionTester intersectionTester;

    Sphere sphere(vector(0, 0, 0), 2);
    Sphere anotherSphere(vector(0, 6, 0), 2);
    Plane plane(vector(0, 0, 0), vector(0, 1, 0));

    REQUIRE(intersectionTester.intersects((Geometry &)plane, (Geometry &)sphere));
    REQUIRE(!intersectionTester.intersects((Geometry &)plane, (Geometry &)anotherSphere));
}

TEST_CASE("Aabb Intersections")
{
    CollisionTester intersectionTester;

    Sphere sphere(vector(0, 0, 0), 2);
    AABB aabb(vector(1, 0, 0), vector(1, 1, 1));

    REQUIRE(intersectionTester.intersects((Geometry &)aabb, (Geometry &)sphere));
    aabb.setOrigin(vector(4, 0, 0));
    REQUIRE(!intersectionTester.intersects((Geometry &)aabb, (Geometry &)sphere));
}

TEST_CASE("Sphere Contacts")
{
    CollisionTester intersectionTester;

    real radius = 2.0f;
    Sphere sphere(vector(-1, 0, 0), radius);
    Sphere anotherSphere(vector(2, 1, 1), radius);

//        Plane plane(vector(0, 0, 0), vector(0, 1, 0));
//        AABB aabb(vector(-1, 0, 0), vector(1, 1, 1));

    std::vector<GeometryContact> contacts = intersectionTester.detectCollision((Geometry &)sphere, (Geometry &)anotherSphere);
    REQUIRE(!contacts.empty());
    GeometryContact contact = *contacts.begin();

    REQUIRE((void *)&sphere == (void *)contact.getGeometryA());
    REQUIRE((void *)&anotherSphere == (void *)contact.getGeometryB());

    vector expectedNormal = (sphere.getOrigin() - anotherSphere.getOrigin()).normalizado();
    real expectedPenetration = (radius + radius) - (sphere.getOrigin() - anotherSphere.getOrigin()).modulo();

    REQUIRE(expectedNormal == contact.getNormal());
    REQUIRE(expectedPenetration == contact.getPenetration());


    anotherSphere.setOrigin(vector(0, 6, 0));
    contacts = intersectionTester.detectCollision((Geometry &)sphere, (Geometry &)anotherSphere);
    REQUIRE(contacts.empty());
}

TEST_CASE("Sphere AABB Intersection") {
  CollisionTester intersectionTester;

  Sphere sphere(vector(-7.23, 40.98, 0.00), 10);
  AABB aabb(vector(0.00, 50.00, 0.00), vector(80.00, 10.00, 10.00));

  REQUIRE(intersectionTester.intersects(sphere, aabb));
}
