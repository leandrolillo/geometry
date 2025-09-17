#include <catch2/catch_test_macros.hpp>
#include "Geometry.h"
#include "CollisionTester.h"

TEST_CASE("Geometry Test case")
{
  REQUIRE(1 == 1);
}

TEST_CASE("Sphere Methods")
{
  Sphere sphere(vector(0, 0, 0), 2);

  CHECK(sphere.contains(vector(1, 1, 1).normalizado() * 1.9));
  CHECK(!sphere.contains(vector(1, 1, 1).normalizado() * 2.1));
}

TEST_CASE("Sphere Intersections")
{
  CollisionTester intersectionTester;

  Sphere sphere(vector(0, 0, 0), 2);
  Sphere anotherSphere(vector(3, 1, 1), 2);
  Plane plane(vector(0, 0, 0), vector(0, 1, 0));

  AABB aabb(vector(-1, 0, 0), vector(1, 1, 1));

  CHECK(intersectionTester.intersects((Geometry& )sphere, (Geometry& )anotherSphere));
  CHECK(intersectionTester.intersects((Geometry& )sphere, (Geometry& )plane));
  CHECK(intersectionTester.intersects((Geometry& )sphere, (Geometry& )aabb));

  anotherSphere.setOrigin(vector(0, 6, 0));
  CHECK(!intersectionTester.intersects((Geometry& )sphere, (Geometry& )anotherSphere));
  CHECK(!intersectionTester.intersects((Geometry& )anotherSphere, (Geometry& )plane));
  CHECK(!intersectionTester.intersects((Geometry& )anotherSphere, (Geometry& )aabb));
}

TEST_CASE("Plane Intersections")
{
  CollisionTester intersectionTester;

  Sphere sphere(vector(0, 0, 0), 2);
  Sphere anotherSphere(vector(0, 6, 0), 2);
  Plane plane(vector(0, 0, 0), vector(0, 1, 0));

  CHECK(intersectionTester.intersects((Geometry& )plane, (Geometry& )sphere));
  CHECK(!intersectionTester.intersects((Geometry& )plane, (Geometry& )anotherSphere));
}

TEST_CASE("Aabb Sphere Intersections")
{
  CollisionTester intersectionTester;

  Sphere sphere(vector(0, 0, 0), 2);
  AABB aabb(vector(1, 0, 0), vector(1, 1, 1));

  CHECK(intersectionTester.intersects((Geometry& )aabb, (Geometry& )sphere));
  aabb.setOrigin(vector(4, 0, 0));
  CHECK(!intersectionTester.intersects((Geometry& )aabb, (Geometry& )sphere));
}

TEST_CASE("Sphere AABB Intersection (troubleshooting bug)") {
  CollisionTester intersectionTester;

  Sphere sphere(vector(-7.23, 40.98, 0.00), 10);
  AABB aabb(vector(0.00, 50.00, 0.00), vector(80.00, 10.00, 10.00));

  Plane plane(vector(0.0, 50.0, 0.0), vector(0, 1, 0));

  CHECK(intersectionTester.intersects(sphere, aabb));
  std::vector<GeometryContact> contacts = intersectionTester.detectCollision(sphere, aabb);
  CHECK(contacts.size() > 0);

  CHECK(intersectionTester.intersects(sphere, plane));
  contacts = intersectionTester.detectCollision(sphere, plane);
  CHECK(contacts.size() > 0);


  CHECK(intersectionTester.intersects(aabb, sphere));
  contacts = intersectionTester.detectCollision(aabb, sphere);
  CHECK(contacts.size() > 0);

  CHECK(intersectionTester.intersects(plane, sphere));
  contacts = intersectionTester.detectCollision(plane, sphere);
  CHECK(contacts.size() > 0);

}

TEST_CASE("Aabb Methods")
{
  AABB aabb(vector(0, 0, 0), vector(1, 2, 3));
  CHECK(aabb.contains(vector(0.99, 1.99, 2.99)));
  CHECK(!aabb.contains(vector(1.1, 2.1, 3.1)));

  CHECK(aabb.closestPoint(vector(0.5, 1, 1)) == vector(0.5, 1, 1));
  CHECK(aabb.closestPoint(vector(3, 6, 12)) == vector(1, 2, 3));

  CHECK(aabb.closestSurfacePoint(vector(0.5, 1, 1)) == vector(1, 1, 1));
  CHECK(aabb.closestPoint(vector(3, 6, 12)) == vector(1, 2, 3));

}


TEST_CASE("Aabb Aabb Intersections")
{
  CollisionTester intersectionTester;

  AABB left(vector(0, 0, 0), vector(1, 2, 3));

  /*X axis*/
  AABB right(vector(1.9, 0, 0), vector(1, 1, 1));
  CHECK(intersectionTester.intersects((Geometry& )left, (Geometry& )right));

  right.setOrigin(vector(2.1, 0, 0));
  CHECK(!intersectionTester.intersects((Geometry& )left, (Geometry& )right));

  right.setOrigin(vector(-1.9, 0, 0));
  CHECK(intersectionTester.intersects((Geometry& )left, (Geometry& )right));

  right.setOrigin(vector(-2.1, 0, 0));
  CHECK(!intersectionTester.intersects((Geometry& )left, (Geometry& )right));

  /*Y axis*/
  right.setOrigin(vector(0, 2.9, 0));
  CHECK(intersectionTester.intersects((Geometry& )left, (Geometry& )right));

  right.setOrigin(vector(0, 3.1, 0));
  CHECK(!intersectionTester.intersects((Geometry& )left, (Geometry& )right));

  right.setOrigin(vector(0, -2.9, 0));
  CHECK(intersectionTester.intersects((Geometry& )left, (Geometry& )right));

  right.setOrigin(vector(0, -3.1, 0));
  CHECK(!intersectionTester.intersects((Geometry& )left, (Geometry& )right));

  /*Z axis*/
  right.setOrigin(vector(0, 0, 3.9));
  CHECK(intersectionTester.intersects((Geometry& )left, (Geometry& )right));

  right.setOrigin(vector(0, 0, 4.1));
  CHECK(!intersectionTester.intersects((Geometry& )left, (Geometry& )right));

  right.setOrigin(vector(0, 0, -3.9));
  CHECK(intersectionTester.intersects((Geometry& )left, (Geometry& )right));

  right.setOrigin(vector(0, 0, -4.1));
  CHECK(!intersectionTester.intersects((Geometry& )left, (Geometry& )right));
}

TEST_CASE("Ray Aabb Intersections") {
  CollisionTester intersectionTester;

  AABB aabb(vector(0, 0, 0), vector(1, 2, 3));
  Line line(vector(0, 0, 0), vector(0, 0, -1));

  CHECK(intersectionTester.intersects((Geometry &)aabb, (Geometry &)line));

  line.setOrigin(vector(2, 0, 0));
  CHECK(!intersectionTester.intersects((Geometry &)aabb, (Geometry &)line));

  // x axis
  line.setOrigin(vector(2, 0, 0));
  line.setDirection(vector(-1, 0, 0));
  CHECK(intersectionTester.intersects((Geometry &)aabb, (Geometry &)line));

  line.setOrigin(vector(-2, 0, 0));
  line.setDirection(vector(1, 0, 0));
  CHECK(intersectionTester.intersects((Geometry &)aabb, (Geometry &)line));

  line.setDirection(vector(-1, 0, 0));
  CHECK(!intersectionTester.intersects((Geometry &)aabb, (Geometry &)line));

  // y axis
  line.setOrigin(vector(0, 3, 0));
  line.setDirection(vector(0, -1, 0));
  CHECK(intersectionTester.intersects((Geometry &)aabb, (Geometry &)line));

  line.setOrigin(vector(0, -3, 0));
  line.setDirection(vector(0, 1, 0));
  CHECK(intersectionTester.intersects((Geometry &)aabb, (Geometry &)line));

  line.setDirection(vector(0, -1, 0));
  CHECK(!intersectionTester.intersects((Geometry &)aabb, (Geometry &)line));

  // z axis
  line.setOrigin(vector(0, 0, 4));
  line.setDirection(vector(0, 0 , -1));
  CHECK(intersectionTester.intersects((Geometry &)aabb, (Geometry &)line));

  line.setOrigin(vector(0, 0, -4));
  line.setDirection(vector(0, 0, 1));
  CHECK(intersectionTester.intersects((Geometry &)aabb, (Geometry &)line));

  line.setDirection(vector(0, 0, -1));
  CHECK(!intersectionTester.intersects((Geometry &)aabb, (Geometry &)line));

}

TEST_CASE("Sphere Contacts")
{
  CollisionTester intersectionTester;

  real radius = 2.0f;
  Sphere sphere(vector(-1, 0, 0), radius);
  Sphere anotherSphere(vector(2, 1, 1), radius);

//        Plane plane(vector(0, 0, 0), vector(0, 1, 0));
//        AABB aabb(vector(-1, 0, 0), vector(1, 1, 1));

  std::vector<GeometryContact> contacts = intersectionTester.detectCollision((Geometry&) sphere, (Geometry&) anotherSphere);
  REQUIRE(!contacts.empty());
  GeometryContact contact = *contacts.begin();

  CHECK((void* )&sphere == (void* )contact.getGeometryA());
  CHECK((void* )&anotherSphere == (void* )contact.getGeometryB());

  vector expectedNormal = (sphere.getOrigin() - anotherSphere.getOrigin()).normalizado();
  real expectedPenetration = (radius + radius) - (sphere.getOrigin() - anotherSphere.getOrigin()).modulo();

  CHECK(expectedNormal == contact.getNormal());
  CHECK(expectedPenetration == contact.getPenetration());

  anotherSphere.setOrigin(vector(0, 6, 0));
  contacts = intersectionTester.detectCollision((Geometry&) sphere, (Geometry&) anotherSphere);
  REQUIRE(contacts.empty());
}
