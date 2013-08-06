//
// StreamDisplay.hpp for  in /home/olivie_a//rendu/nao/nao-car/NaoCar/Apps/Remote/Sources
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Fri Mar 15 20:31:08 2013 samuel olivier
// Last update Fri May 24 12:41:00 2013 samuel olivier
//

#ifndef __STREAM_DISPLAY_HPP__
# define __STREAM_DISPLAY_HPP__

# include <QWidget>
# include <QImage>
# include <QPaintEvent>

class StreamDisplay : public QWidget {
public:
  StreamDisplay(QWidget *parent = NULL);
  ~StreamDisplay();

  void setStreamImage(QImage *image);

protected:
  virtual void paintEvent(QPaintEvent *event);

private:
  QImage	*_image;
};

#endif
