/*
 * Geometry.h
 *
 *  Created on: May 9, 2021
 *      Author: leandro
 */

#pragma once

#include "Math3d.h"

enum class GeometryType {
    SPHERE,
    PLANE,
    LINE,
    AABB,
		OOBB,
    HIERARCHY,
    HEIGHTMAP,
		FRUSTUM
};


class Geometry {
  vector origin; //keep this property private and use getOrigin instead.
  bool _status = true;
public:
  Geometry(const vector &origin) {
      this->origin = origin;
  }

  virtual ~Geometry() {}

  virtual const vector& getOrigin() const {
      return this->origin;
  }

  virtual void setOrigin(const vector &origin) {
      this->origin = origin;
  }

  bool getStatus() const {
    return this->_status;
  }

  Geometry &setStatus(bool active) {
    this->_status = active;
    return *this;
  }


  virtual String toString() const {
      return "Geometry(origin: " + origin.toString() + ")";
  }

  virtual GeometryType getType() const = 0;
};

class Sphere: public Geometry {
  real radius;
public:
  Sphere(const vector &origin, real radius) : Geometry(origin) {
      this->radius = radius;
  }

  real getRadius() const {
      return this->radius;
  }

  void setRadius(real radius) {
      this->radius = radius;
  }

  bool contains(const vector &point) const {
    vector delta = getOrigin() - point;
    return delta * delta <= radius * radius;
  }

  String toString() const override {
      return "Sphere(origin: " + this->getOrigin().toString() + ", radius: " + std::to_string(this->radius) + ")";
  }

  GeometryType getType() const override {
      return GeometryType::SPHERE;
  }
};

class Plane: public Geometry {
  vector normal;
public:
  Plane(const vector &origin, const vector &normal) : Geometry(origin) {
      this->normal = normal.normalizado();
  }

  const vector &getNormal() const {
      return this->normal;
  }

  virtual void setNormal(const vector &normal) {
    this->normal = normal;
  }

  String toString() const override {
      return "Plane(origin: " + this->getOrigin().toString("%.6f") + ", normal: " + this->normal.toString("%.6f") + ")";
  }

  GeometryType getType() const override {
      return GeometryType::PLANE;
  }
};

class Line: public Geometry { //Do we need lines or should this really be rays (meaning we ignore negative t in parametric ecuation)
  vector direction;
public:
  Line(const vector &origin, const vector &direction) : Geometry(origin){
      setDirection(direction);
  }

  const vector& getDirection() const {
      return this->direction;
  }

  void setDirection(const vector &direction) {
    this->direction = direction.normalizado();
  }

  String toString() const override {
      return "Line(origin: " + this->getOrigin().toString() + ", dir: " + this->direction.toString() + ")";
  }

  GeometryType getType() const override {
      return GeometryType::LINE;
  }
};

class AABB : public Geometry {
  vector halfSizes;
public:
  AABB(const vector &origin, const vector &halfSizes) : Geometry(origin) {
      this->halfSizes = halfSizes;
  }

  const vector &getHalfSizes() const {
      return this->halfSizes;
  }

  void setHalfSizes(const vector &halfSizes) {
      this->halfSizes = halfSizes;
  }


  /**
   * Returns bottom left back vertex
   */
  vector getMins() const {
    return this->getOrigin() - this->halfSizes;
  }

  /**
   * Returns top right front vertex
   */
  vector getMaxs() const {
    return this->getOrigin() + this->halfSizes;
  }

  /**
   * Returns the bottom left position of the aabb. On the other hand, Origin is the center.
   */
  vector getPosition() const {
    return getMins();
  }

  String toString() const override {
      return "AABB(origin: " + this->getOrigin().toString() + ", halfSizes: " + this->halfSizes.toString() + ")";
  }

  GeometryType getType() const override {
      return GeometryType::AABB;
  }

  bool contains(const vector &point) const {
    vector mins(getMins());
    vector maxs(getMaxs());

    return mins.x <= point.x && point.x <= maxs.x &&
        mins.y <= point.y && point.y <= maxs.y &&
        mins.z <= point.z && point.z <= maxs.z;
  }

  AABB minkowskiDifference(const AABB &right) const {
    vector minA(this->getMins());
    vector maxB(right.getMaxs());

    vector minMD = minA - maxB;
    vector halfSizesMD = this->halfSizes + right.halfSizes;

    return AABB(minMD + halfSizesMD, halfSizesMD);
  }

  vector closestPoint(const vector &target) const {
      vector mins(getMins());
      vector maxs(getMaxs());


      return vector(std::max(mins.x, std::min(target.x, maxs.x)),
              std::max(mins.y, std::min(target.y, maxs.y)),
              std::max(mins.z, std::min(target.z, maxs.z))
              );
  }

  vector closestSurfacePoint(const vector &target) const {
    vector surfacePoint;
    real minDistance = REAL_MAX;

    real faceCoord = this->getOrigin().y + this->halfSizes.y;
    real faceDistanceSq = (faceCoord - target.y) * (faceCoord - target.y);
    //if(faceDistanceSq < minDistance) {
      minDistance = faceDistanceSq;
      surfacePoint = vector(target.x, faceCoord, target.z);
    //}

    faceCoord = this->getOrigin().y - this->halfSizes.y;
    faceDistanceSq = (faceCoord - target.y) * (faceCoord - target.y);
    if(faceDistanceSq <= minDistance) {
      minDistance = faceDistanceSq;
      surfacePoint = vector(target.x, faceCoord, target.z);
    }

    faceCoord = this->getOrigin().x - this->halfSizes.x;
    faceDistanceSq = (faceCoord - target.x) * (faceCoord - target.x);
    if(faceDistanceSq <= minDistance) {
      minDistance = faceDistanceSq;
      surfacePoint = vector(faceCoord, target.y, target.z);
    }

    faceCoord = this->getOrigin().x + this->halfSizes.x;
    faceDistanceSq = (faceCoord - target.x) * (faceCoord - target.x);
    if(faceDistanceSq < minDistance) {
      minDistance = faceDistanceSq;
      surfacePoint = vector(faceCoord, target.y, target.z);
    }

    faceCoord = this->getOrigin().z - this->halfSizes.z;
    faceDistanceSq = (faceCoord - target.z) * (faceCoord - target.z);
    if(faceDistanceSq < minDistance) {
      minDistance = faceDistanceSq;
      surfacePoint = vector(target.x, target.y, faceCoord);
    }

    faceCoord = this->getOrigin().z + this->halfSizes.z;
    faceDistanceSq = (faceCoord - target.z) * (faceCoord - target.z);
    if(faceDistanceSq < minDistance) {
      minDistance = faceDistanceSq;
      surfacePoint = vector(target.x, target.y, faceCoord);
    }

		return surfacePoint;
	}
};

/**
 * Loose ends: the bounding volume should be refreshed upon children transformations
 */
class HierarchicalGeometry : public Geometry {
  std::unique_ptr<Geometry> boundingVolume;
  std::vector<std::unique_ptr<Geometry>> children;
public:
  HierarchicalGeometry(std::unique_ptr<Geometry> boundingVolume) :
    Geometry(boundingVolume->getOrigin()), boundingVolume(std::move(boundingVolume)) {
  }
  HierarchicalGeometry(std::unique_ptr<Geometry> boundingVolume, std::unique_ptr<Geometry> child) :
    HierarchicalGeometry(std::move(boundingVolume)) {
    this->addChildren(std::move(child));
}
  const vector& getOrigin() const override {
      return this->boundingVolume->getOrigin();
  }

  void setOrigin(const vector &origin) override {
      vector delta = origin - this->getOrigin();

      this->boundingVolume->setOrigin(origin);

      for(auto &child : children) {
          child->setOrigin(child->getOrigin() + delta);
      }
  }

  void addChildren(std::unique_ptr<Geometry> children) {
      this->children.push_back(std::move(children));

  }

  String toString() const override {
      return "HierarchicalGeometry(origin: " + this->getOrigin().toString() + ", children: " + std::to_string(this->children.size()) + ")";
  }

  GeometryType getType() const override {
      return GeometryType::HIERARCHY;
  }

  const Geometry &getBoundingVolume() const {
      return *this->boundingVolume.get();
  }

  const std::vector<std::unique_ptr<Geometry>> &getChildren() const {
      return this->children;
  }
};


/**
 * Base class for Height Maps - Basically it provides
 *  - a real multivariable function that maps x, z coordinates to a height, thus forming 3d surface.
 *  - a vectorial function that maps (x,z) with the normal at point (x, height, z)
 *
 * It is constrained by width and depth (also height but not sure this should be here - it's here for the aabb)
 * Implementations could be bitmap based, geographical, etc.
*/
class HeightMap {
public:
  //TODO: review if this is required or we should split aabb from heightmapgeometry and use composition instead.
  virtual real getWidth() const = 0;
  virtual real getHeight() const = 0;
  virtual real getDepth() const = 0;

  /**
   * Returns y coordinate corresponding to point (x, height, z) on the surface
   */
  virtual real heightAt(real x, real z) const = 0;

  /**
   * Returns the normal at a  given (x, z)
   */
  virtual vector normalAt(real x, real z) const = 0;

  /**
   * Returns the point (x, height, z) on the surface
   */
  virtual vector positionAt(real x, real z) const {
    return vector(x, heightAt(x, z), z);
  }

  virtual String toString() const {
    return String("HeightMap");
  }
};

class HeightMapGeometry : public AABB {
  const HeightMap &heightMap;
public:
  HeightMapGeometry(const vector &position, const HeightMap &heightMap) :
    AABB(position + vector(heightMap.getWidth() * 0.5, heightMap.getHeight() * 0.5, heightMap.getDepth() * 0.5),
      vector(heightMap.getWidth() * 0.5, heightMap.getHeight() * 0.5, heightMap.getDepth() * 0.5)), heightMap(heightMap) {
  }

  const HeightMap &getHeightMap() const {
    return this->heightMap;
  }

  GeometryType getType() const override {
      return GeometryType::HEIGHTMAP;
  }


  real heightAt(real x, real z) const {
    return this->heightMap.heightAt(x - this->getPosition().x, z - this->getPosition().z);
  }

  vector normalAt(real x, real z) const {
    return this->heightMap.normalAt(x - this->getPosition().x, z - this->getPosition().z);
  }
};

/**
 * Loose ends
 *  - contact and collision tests are gona be generic same as hierarchy - maybe could use a single method and add pairs automatically
 * 	- we should check the planes create a closed solid
 */
class Frustum : public Geometry {
	std::vector<Plane> halfSpaces;
public:
	Frustum(const std::vector<Plane> &halfSpaces) : Geometry(vector(0, 0, 0)) {
		this->halfSpaces = halfSpaces;
	}

	GeometryType getType() const override {
		return GeometryType::FRUSTUM;
	}

	const std::vector<Plane>& getHalfSpaces() const {
		return halfSpaces;
	}

	Plane &getHalfSpace(unsigned int index) {
		return halfSpaces[index];
	}
};
