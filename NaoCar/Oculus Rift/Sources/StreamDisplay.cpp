//
// StreamDisplay.cpp for  in /home/olivie_a//rendu/nao/nao-car/NaoCar/Apps/Remote/Sources
//
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
//
// Started on  Fri Mar 15 21:16:34 2013 samuel olivier
// Last update Sat Mar 16 02:05:04 2013 samuel olivier
//

#include <QPainter>
#include <QRect>
#include <QDebug>

#include "StreamDisplay.hpp"

StreamDisplay::StreamDisplay(QWidget *parent) : QWidget(parent), _image(NULL) {
}

StreamDisplay::~StreamDisplay() {
}

void	StreamDisplay::setStreamImage(QImage *image) {
    _image = image;
    repaint();
}

void	StreamDisplay::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    
    painter.setPen(Qt::blue);
    painter.setFont(QFont("Arial", 30));
    if (_image) {
        painter.drawImage(rect(), *_image);
    }
}

