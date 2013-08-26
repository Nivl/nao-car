//
// HandDescription.cpp for  in /home/olivie_a//rendu/nao/nao-car/NaoCar/VRemote/Sources
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Fri Aug  9 20:43:47 2013 samuel olivier
// Last update Sun Aug 11 16:08:56 2013 samuel olivier
//

#include "HandDescription.hpp"

HandDescription::HandDescription(void) {
  setPitchRequested(false);
  setRollRequested(false);
  setYawRequested(false);
  setXRequested(false);
  setYRequested(false);
  setZRequested(false);
}

HandDescription::~HandDescription(void) {

}


void        HandDescription::setPitch(float pitch) {
  setValue(PITCH_ID, pitch);
}

float       HandDescription::pitch(void) const {
  return getValue(PITCH_ID);
}

void        HandDescription::setPitchRequested(bool isRequested) {
  setValueRequested(PITCH_ID, isRequested);
}

bool        HandDescription::isPitchRequested(void) const {
  return getValueRequested(PITCH_ID);
}


void        HandDescription::setRoll(float roll) {
  setValue(ROLL_ID, roll);
}

float       HandDescription::roll(void) const {
  return getValue(ROLL_ID);
}

void        HandDescription::setRollRequested(bool isRequested) {
  setValueRequested(ROLL_ID, isRequested);
}

bool        HandDescription::isRollRequested(void) const {
  return getValueRequested(ROLL_ID);
}


void        HandDescription::setYaw(float yaw) {
  setValue(YAW_ID, yaw);
}

float       HandDescription::yaw(void) const {
  return getValue(YAW_ID);
}

void        HandDescription::setYawRequested(bool isRequested) {
  setValueRequested(YAW_ID, isRequested);
}

bool        HandDescription::isYawRequested(void) const {
  return getValueRequested(YAW_ID);
}


void        HandDescription::setX(float x) {
  setValue(X_ID, x);
}

float       HandDescription::x(void) const {
  return getValue(X_ID);
}

void        HandDescription::setXRequested(bool isRequested) {
  setValueRequested(X_ID, isRequested);
}

bool        HandDescription::isXRequested(void) const {
  return getValueRequested(X_ID);
}


void        HandDescription::setY(float y) {
  setValue(Y_ID, y);
}

float       HandDescription::y(void) const {
  return getValue(Y_ID);
}

void        HandDescription::setYRequested(bool isRequested) {
  setValueRequested(Y_ID, isRequested);
}

bool        HandDescription::isYRequested(void) const {
  return getValueRequested(Y_ID);
}


void        HandDescription::setZ(float z) {
  setValue(Z_ID, z);
}

float       HandDescription::z(void) const {
  return getValue(Z_ID);
}

void        HandDescription::setZRequested(bool isRequested) {
  setValueRequested(Z_ID, isRequested);
}

bool        HandDescription::isZRequested(void) const {
  return getValueRequested(Z_ID);
}


void        HandDescription::setValue(QString const& id, float value) {
  _values[id] = QPair<float, bool>(value, true);
}

void        HandDescription::setValueRequested(QString const& id,
                                                bool isRequested) {
  if (_values.contains(id))
    _values[id].second = isRequested;
  else
  _values[id] = QPair<float, bool>(0, isRequested);
}

float        HandDescription::getValue(QString const& id) const {
  if (_values.contains(id))
    return _values[id].first;
  else
    return 0;
}

bool        HandDescription::getValueRequested(QString const& id) const {
  if (_values.contains(id))
    return _values[id].second;
  else
    return false;
}
