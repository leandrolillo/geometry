#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "mathMatchers.h"

#include "Geometry.h"
#include "CollisionTester.h"

class FlatHeightMap : public HeightMap {
public:
  real getWidth() const override { return 10.0; }
  real getHeight() const override { return 5.0; }
  real getDepth() const override { return 10.0; }
  real heightAt(real x, real z) const override {
    (void)x;
    (void)z;
    return 0.0;
  }
  vector normalAt(real x, real z) const override {
    (void)x;
    (void)z;
    return vector(0, 1, 0);
  }
};

TEST_CASE("Geometry Test case")
{
  REQUIRE(1 == 1);
}

TEST_CASE("Sphere Methods")
{
  Sphere sphere(vector(0, 0, 0), 2);

  CHECK(sphere.contains(vector(1, 1, 1).normalizado() * 1.9));
  CHECK(!sphere.contains(vector(1, 1, 1).normalizado() * 2.1));

  GeometryContact contact(null, null, vector(), vector(), 0.0);

  unsigned int onCollisionHandlerInvokation = 0;
  sphere.onCollision(contact);
  CHECK(onCollisionHandlerInvokation == 0);

  sphere.setOnCollisionHandler([&onCollisionHandlerInvokation](GeometryContact &contact) { onCollisionHandlerInvokation++; });
  sphere.onCollision(contact);
  CHECK(onCollisionHandlerInvokation == 1);
}

TEST_CASE("AABB Methods") {
  AABB aabb(vector(0, 0, 0), vector(1, 2, 3));
  CHECK(aabb.getMins() == vector(-1, -2, -3));
  CHECK(aabb.getMaxs() == vector(1, 2, 3));
  CHECK(aabb.getTopLeft() == vector(-1, -2, -3));

  aabb.setTopLeft(vector(0, 0, 0));
  CHECK(aabb.getOrigin() == vector(1, 2, 3));

  aabb.setOrigin(vector(0, 0, 0));
  CHECK(aabb.contains(vector(-0.99, -1.99, -2.99)));
  CHECK(!aabb.contains(vector(-1.1, -1.99, -2.99)));
  CHECK(!aabb.contains(vector(-0.99, -2.1, -2.99)));
  CHECK(!aabb.contains(vector(-0.99, -1.99, -3.1)));

  //contains
  CHECK(aabb.contains(vector(0.99, 1.99, 2.99)));
  CHECK(!aabb.contains(vector(1.1, 1.99, 2.99)));
  CHECK(!aabb.contains(vector(0.99, 2.1, 2.99)));
  CHECK(!aabb.contains(vector(0.99, 1.99, 3.1)));

  //closestPoint
  CHECK(aabb.closestPoint(vector(0.5, 1, 1)) == vector(0.5, 1, 1));
  CHECK(aabb.closestPoint(vector(3, 6, 12)) == vector(1, 2, 3));

  //closestSurfacePoint
  CHECK(aabb.closestSurfacePoint(vector(0.5, 1, 1)) == vector(1, 1, 1));
  CHECK(aabb.closestPoint(vector(3, 6, 12)) == vector(1, 2, 3));

  AABB right(vector(1, 1, 1), vector(1, 2, 3));
  AABB minkowskiDifference = aabb.minkowskiDifference(right);

  //vector center = vector(-1, -2, -3) - vector(2, 3, 4) + vector(2, 4, 6) = vector(-1, -1, -1);
  CHECK(minkowskiDifference.getOrigin() == vector(-1.0, -1.0, -1.0));
  CHECK(minkowskiDifference.getHalfSizes() == vector(2.0, 4.0, 6.0));
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
  GeometryContact &contact = contacts.front();

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

TEST_CASE("Aabb Aabb Contacts")
{
  CollisionTester intersectionTester;

  AABB left(vector(0, 0, 0), vector(1, 2, 3));

  /*X axis*/
  AABB right(vector(1.9, 0, 0), vector(1, 1, 1));
  std::vector<GeometryContact> contacts = intersectionTester.detectCollision(left, right);
  REQUIRE(!contacts.empty());
  CHECK(&left == contacts.front().getGeometryA());
  CHECK(&right == contacts.front().getGeometryB());
  CHECK_THAT(contacts.front().getPenetration(), Catch::Matchers::WithinAbs(0.1, 0.2));
  CHECK_THAT(contacts.front().getNormal(), EqualsVector(vector(-1, 0, 0)));

  right.setOrigin(vector(2.1, 0, 0));
  REQUIRE(intersectionTester.detectCollision(left, right).empty());

  right.setOrigin(vector(-1.9, 0, 0));
  contacts = intersectionTester.detectCollision(left, right);
  REQUIRE(!contacts.empty());
  CHECK(&left == contacts.front().getGeometryA());
  CHECK(&right == contacts.front().getGeometryB());
  CHECK_THAT(contacts.front().getPenetration(), Catch::Matchers::WithinAbs(0.1, 0.2));
  CHECK_THAT(contacts.front().getNormal(), EqualsVector(vector(1, 0, 0)));

  right.setOrigin(vector(-2.1, 0, 0));
  REQUIRE(intersectionTester.detectCollision(left, right).empty());

  //  /*Y axis*/
  right.setOrigin(vector(0, 2.9, 0));
  contacts = intersectionTester.detectCollision(left, right);
  REQUIRE(!contacts.empty());
  CHECK(&left == contacts.front().getGeometryA());
  CHECK(&right == contacts.front().getGeometryB());
  CHECK_THAT(contacts.front().getPenetration(), Catch::Matchers::WithinAbs(0.1, 0.2));
  CHECK_THAT(contacts.front().getNormal(), EqualsVector(vector(0, -1, 0)));

  right.setOrigin(vector(0, 3.1, 0));
  REQUIRE(intersectionTester.detectCollision(left, right).empty());

  right.setOrigin(vector(0, -2.9, 0));
  contacts = intersectionTester.detectCollision(left, right);
  REQUIRE(!contacts.empty());
  CHECK(&left == contacts.front().getGeometryA());
  CHECK(&right == contacts.front().getGeometryB());
  CHECK_THAT(contacts.front().getPenetration(), Catch::Matchers::WithinAbs(0.1, 0.2));
  CHECK_THAT(contacts.front().getNormal(), EqualsVector(vector(0, 1, 0)));

  right.setOrigin(vector(0, -3.1, 0));
  REQUIRE(intersectionTester.detectCollision(left, right).empty());

  /*Z axis*/
  right.setOrigin(vector(0, 0, 3.9));
  contacts = intersectionTester.detectCollision(left, right);
  REQUIRE(!contacts.empty());
  CHECK(&left == contacts.front().getGeometryA());
  CHECK(&right == contacts.front().getGeometryB());
  CHECK_THAT(contacts.front().getPenetration(), Catch::Matchers::WithinAbs(0.1, 0.2));
  CHECK_THAT(contacts.front().getNormal(), EqualsVector(vector(0, 0, -1)));

  right.setOrigin(vector(0, 0, 4.1));
  REQUIRE(intersectionTester.detectCollision(left, right).empty());

  right.setOrigin(vector(0, 0, -3.9));
  contacts = intersectionTester.detectCollision(left, right);
  REQUIRE(!contacts.empty());
  CHECK(&left == contacts.front().getGeometryA());
  CHECK(&right == contacts.front().getGeometryB());
  CHECK_THAT(contacts.front().getPenetration(), Catch::Matchers::WithinAbs(0.1, 0.2));
  CHECK_THAT(contacts.front().getNormal(), EqualsVector(vector(0, 0, 1)));

  right.setOrigin(vector(0, 0, -4.1));
  REQUIRE(intersectionTester.detectCollision(left, right).empty());
}

TEST_CASE("Hierarchy and frustum branches")
{
  CollisionTester collisionTester;

  Sphere querySphere(vector(0, 0, 0), 2.0);
  auto hierarchy = std::make_unique<HierarchicalGeometry>(
      std::make_unique<Sphere>(vector(0, 0, 0), 10.0),
      std::make_unique<Sphere>(vector(0, 0, 0), 1.5));
  hierarchy->addChildren(std::make_unique<Sphere>(vector(20, 20, 20), 1.0));

  CHECK(collisionTester.intersects(querySphere, *hierarchy));
  auto contacts = collisionTester.detectCollision(querySphere, *hierarchy);
  REQUIRE(contacts.size() == 1);

  hierarchy->setOrigin(vector(100, 100, 100));
  CHECK(!collisionTester.intersects(querySphere, *hierarchy));
  CHECK(collisionTester.detectCollision(querySphere, *hierarchy).empty());

  std::vector<Plane> planes{
      Plane(vector(0, 0, 0), vector(1, 0, 0)),
      Plane(vector(0, 0, 0), vector(0, 1, 0)),
      Plane(vector(0, 0, 0), vector(0, 0, 1))
  };
  Frustum frustum(planes);
  CHECK(collisionTester.intersects(querySphere, frustum));

  Sphere farSphere(vector(100, 0, 0), 1.0);
  CHECK(!collisionTester.intersects(farSphere, frustum));
}

TEST_CASE("Heightmap collisions and unsupported pair branches")
{
  CollisionTester collisionTester;
  FlatHeightMap flatHeightMap;
  HeightMapGeometry heightMapGeometry(vector(0, 0, 0), flatHeightMap);

  Sphere closeSphere(vector(2.0, 1.0, 2.0), 2.0);
  Sphere farSphere(vector(2.0, 5.0, 2.0), 1.0);

  CHECK(collisionTester.intersects(closeSphere, heightMapGeometry));
  CHECK(!collisionTester.intersects(farSphere, heightMapGeometry));

  auto contacts = collisionTester.detectCollision(closeSphere, heightMapGeometry);
  REQUIRE(contacts.size() == 1);
  CHECK(contacts.front().getGeometryA() == &closeSphere);
  CHECK(contacts.front().getGeometryB() == &heightMapGeometry);
  CHECK_THAT(contacts.front().getNormal(), EqualsVector(vector(0, 1, 0)));

  CHECK(collisionTester.detectCollision(farSphere, heightMapGeometry).empty());

  Plane plane(vector(0, 0, 0), vector(0, 1, 0));
  CHECK(!collisionTester.intersects(plane, plane));
  CHECK(collisionTester.detectCollision(plane, plane).empty());
}

TEST_CASE("Contact utility branches")
{
  Sphere sphereA(vector(0, 0, 0), 1.0);
  Sphere sphereB(vector(1, 0, 0), 1.0);
  GeometryContact contact(&sphereA, &sphereB, vector(1, 2, 3), vector(0, 1, 0), -0.5);
  CHECK(!contact.isIntersecting());

  GeometryContact reversed = contact.reverse();
  CHECK(reversed.getGeometryA() == contact.getGeometryB());
  CHECK(reversed.getGeometryB() == contact.getGeometryA());
  CHECK_THAT(reversed.getNormal(), EqualsVector(vector(0, -1, 0)));
}
