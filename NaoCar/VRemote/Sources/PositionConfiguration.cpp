//
// PositionConfiguration.cpp for  in /home/olivie_a//rendu/nao/nao-car/NaoCar/VRemote/Sources
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Fri Aug  9 21:13:16 2013 samuel olivier
// Last update Sun Aug 11 22:02:02 2013 samuel olivier
//

#include "PositionConfiguration.hpp"

#include <QDomDocument>
#include <QDebug>
#include <QFile>

#define RETURN_IF_FALSE(b, str) if (!b) return false;

PositionConfiguration::PositionConfiguration(QString const& filename, QString const& name)
    : _name(name) {
    QDomDocument  doc(filename);
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return;
    if (!doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();
    QDomNodeList  hands = doc.elementsByTagName("hand");

    for (int i = 0; i < hands.size(); ++i) {
        _parseHand(hands.at(i).toElement());
    }
}

PositionConfiguration::~PositionConfiguration(void) {  
}

bool  PositionConfiguration::matchConfiguration(HandList const& hands) {
    if (hands.count() < _handDescriptions.size())
        return false;
    QList<int> matchedDescription;

    for (int i = 0; i < hands.count() &&
         matchedDescription.size() < _handDescriptions.size(); ++i) {
        Hand const& current = hands[i];

        int matchedIdx = _matchOneConfiguration(current, matchedDescription);
        if (matchedIdx >= 0) {
            matchedDescription << matchedIdx;
        }
    }
    return matchedDescription.size() == _handDescriptions.size();
}

int  PositionConfiguration::_matchOneConfiguration(Hand const& hand,
                                                   QList<int> const& excepted) {
    int   idx = -1;
    foreach (auto current, _handDescriptions) {
        ++idx;
        if (excepted.contains(idx))
            continue ;
        if (_matchConfiguration(hand, current.first,
                                &PositionConfiguration::_matchMinimumConfiguration, "minimum") &&
                _matchConfiguration(hand, current.second,
                                    &PositionConfiguration::_matchMaximumConfiguration, "maximum"))
            return idx;
    }
    return -1;
}

void PositionConfiguration::_parseHand(QDomElement const& element) {
    QPair<HandDescription, HandDescription> hand;
    QDomElement minimum = element.elementsByTagName("minimum").at(0).toElement();
    QDomElement maximum = element.elementsByTagName("maximum").at(0).toElement();

    QDomNodeList values = minimum.childNodes();
    for (int i = 0; i < values.size(); ++i) {
        QDomElement current = values.at(i).toElement();

        hand.first.setValue(current.tagName().append("_id"),
                            current.text().toFloat());
    }
    values = maximum.childNodes();
    for (int i = 0; i < values.size(); ++i) {
        QDomElement current = values.at(i).toElement();

        hand.second.setValue(current.tagName().append("_id"),
                             current.text().toFloat());
    }
    _handDescriptions.append(hand);
}

bool PositionConfiguration::_matchMinimumConfiguration(float value,
                                                       float minimum) {
    return value >= minimum;
}

bool PositionConfiguration::_matchMaximumConfiguration(float value,
                                                       float maximum) {
    return value <= maximum;
}

bool PositionConfiguration::_matchConfiguration(Hand const& hand,
                                                HandDescription const& d,
                                                checkFunction f,
                                                QString const& type) {
    const Vector palmPosition = hand.palmPosition();
    const Vector normal = hand.palmNormal();
    const Vector direction = hand.direction();
    float x = palmPosition.x,
            y = palmPosition.y,
            z = palmPosition.z,
            pitch = direction.pitch() * RAD_TO_DEG,
            roll = normal.roll() * RAD_TO_DEG,
            yaw = direction.yaw() * RAD_TO_DEG;

    if (d.isXRequested())
        RETURN_IF_FALSE((this->*f)(x, d.x()), "X");
    if (d.isYRequested())
        RETURN_IF_FALSE((this->*f)(y, d.y()), "Y");
    if (d.isZRequested())
        RETURN_IF_FALSE((this->*f)(z, d.z()), "Z");
    if (d.isPitchRequested())
        RETURN_IF_FALSE((this->*f)(pitch, d.pitch()), "Pitch");
    if (d.isRollRequested())
        RETURN_IF_FALSE((this->*f)(roll, d.roll()), "Roll");
    if (d.isYawRequested())
        RETURN_IF_FALSE((this->*f)(yaw, d.yaw()), "Yaw");
    return true;
}
