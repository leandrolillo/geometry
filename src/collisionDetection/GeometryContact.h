/*
 * GeometryContact.h
 *
 *  Created on: May 18, 2021
 *      Author: leandro
 */

#pragma once

#include<Geometry.h>
#include "BaseContact.h"

class Geometry;

class GeometryContact : public BaseContact {
private:
  const Geometry *geometryA;
  const Geometry *geometryB;

public:
  GeometryContact(const Geometry *geometryA, const Geometry *geometryB, const vector &intersection, const vector &normal, real restitution, real penetration = 0.0) : BaseContact (intersection, normal, restitution, penetration){
      this->geometryA = geometryA;
      this->geometryB = geometryB;
  }

  const Geometry *getGeometryA() const {
      return this->geometryA;
  }

  const Geometry *getGeometryB() const {
      return this->geometryB;
  }

  GeometryContact reverse() const {
      return GeometryContact(this->geometryB, this->geometryA, intersection, normal * -1, restitution, penetration);
  }
};
