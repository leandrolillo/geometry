# geometry

`geometry` is a small C++17 geometry and collision-detection library built with CMake.

## What it includes

- Geometric primitives:
  - `Sphere`
  - `Plane`
  - `Line`
  - `AABB`
  - `HierarchicalGeometry`
  - `HeightMapGeometry`
  - `Frustum`
- Collision queries through `CollisionTester`
  - intersection checks
  - contact generation
- Contact types:
  - `BaseContact`
  - `GeometryContact`

The library is exposed as an interface target, with most functionality implemented in headers under `/src`.

## Dependencies

The project fetches its dependencies with CMake:

- [`math`](https://github.com/leandrolillo/math.git)
- [`Catch2`](https://github.com/catchorg/Catch2) for tests

An internet connection is required the first time CMake configures the project.

## Build

From the repository root:

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
```

## Run tests

After building:

```bash
cd build
ctest --output-on-failure
```

## Project layout

```text
/src
  /geometry             Core geometry types
  /collisionDetection   Intersection and contact logic
/test
  GeometryTests.cpp     Unit tests
  /matchers             Test helpers
```

## Notes

- The minimum CMake version is `3.22`.
- The project uses the C++17 standard.
- Existing tests currently focus on geometry primitives and collision behavior.