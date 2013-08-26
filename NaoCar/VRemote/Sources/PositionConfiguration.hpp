//
// PositionConfiguration.hpp for  in /home/olivie_a//rendu/nao/nao-car/NaoCar/VRemote/Sources
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Fri Aug  9 21:09:08 2013 samuel olivier
// Last update Sun Aug 11 21:47:42 2013 samuel olivier
//

#ifndef _POSITION_CONFIGURATION_HPP_
# define _POSITION_CONFIGURATION_HPP_

# include <QList>
# include <QPair>
# include <Leap.h>
# include <QDomElement>

# include "HandDescription.hpp"

using namespace Leap;

class PositionConfiguration {
public:
    PositionConfiguration(QString const& filename = QString(), QString const& name = QString());
    ~PositionConfiguration(void);

    bool  matchConfiguration(HandList const& hands);

private:
    typedef bool (PositionConfiguration::*checkFunction)(float, float);

    int  _matchOneConfiguration(Hand const& hand, QList<int> const& excepted);
    void _parseHand(QDomElement const& element);

    bool _matchMinimumConfiguration(float value, float minimum);
    bool _matchMaximumConfiguration(float value, float maximum);
    bool _matchConfiguration(Hand const& hand,
                             HandDescription const& description,
                             checkFunction f, QString const& type);

    QList<QPair<HandDescription, HandDescription> >       _handDescriptions;
    QString   _name;
};

#endif
