//
// HandDescription.hpp for  in /home/olivie_a//rendu/nao/nao-car/NaoCar/VRemote/Sources
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Fri Aug  9 20:42:20 2013 samuel olivier
// Last update Sun Aug 11 16:08:40 2013 samuel olivier
//

#ifndef _HAND_DESCRIPTION_HPP_
# define _HAND_DESCRIPTION_HPP_

# include <QHash>
# include <QString>

# define PITCH_ID "pitch_id"
# define ROLL_ID "roll_id"
# define YAW_ID "yaw_id"
# define X_ID "x_id"
# define Y_ID "y_id"
# define Z_ID "z_id"

class HandDescription {
public:
  HandDescription(void);
  ~HandDescription(void);

  void        setPitch(float pitch);
  float       pitch(void) const;
  void        setPitchRequested(bool isRequested);
  bool        isPitchRequested(void) const;

  void        setRoll(float roll);
  float       roll(void) const;
  void        setRollRequested(bool isRequested);
  bool        isRollRequested(void) const;

  void        setYaw(float yaw);
  float       yaw(void) const;
  void        setYawRequested(bool isRequested);
  bool        isYawRequested(void) const;
    
  void        setX(float x);
  float       x(void) const;
  void        setXRequested(bool isRequested);
  bool        isXRequested(void) const;
    
  void        setY(float y);
  float       y(void) const;
  void        setYRequested(bool isRequested);
  bool        isYRequested(void) const;
    
  void        setZ(float z);
  float       z(void) const;
  void        setZRequested(bool isRequested);
  bool        isZRequested(void) const;

  void  setValue(QString const& id, float value);
  void  setValueRequested(QString const& id, bool isRequested);
  float getValue(QString const& id) const;
  bool  getValueRequested(QString const& id) const;

private:
  QHash<QString, QPair<float, bool> > _values;
};

#endif
