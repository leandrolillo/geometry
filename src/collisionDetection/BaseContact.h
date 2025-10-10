/*
 * Contact.h
 *
 *  Created on: May 18, 2021
 *      Author: leandro
 */

#pragma once
#include<Math3d.h>

class BaseContact {
protected:
  vector normal;
  vector intersection; //we might want to have different intersections per colliding object
  real penetration {0};


public:
  BaseContact(const vector &intersection, const vector &normal, real penetration = 0.0) {
    this->intersection = intersection;
    this->normal = normal;
    this->penetration = penetration;
  }

  const vector& getNormal() const {
    return this->normal;
  }

  const vector& getIntersection() const {
    return this->intersection;
  }

  real getPenetration() const {
    return this->penetration;
  }

  bool isIntersecting() const {
    return this->penetration >= 0.0;
  }
};
