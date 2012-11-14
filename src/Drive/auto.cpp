//
// test.cpp for  in /home/jochau_g//Desktop/opencv
// 
// Made by gael jochaud-du-plessix
// Login   <jochau_g@epitech.net>
// 
// Started on  Fri Sep 28 13:59:38 2012 gael jochaud-du-plessix
// Last update Tue Nov 13 22:42:14 2012 samuel olivier
//

#include <alcommon/almodulecore.h>
#include <alcommon/almoduleinfo.h>
#include <alcommon/albroker.h>
#include "opencv2/opencv.hpp"

#include <gst/gst.h>
#include <glib.h>
#include <gst/app/gstappsink.h>

#include <iostream>
#include <math.h>
#include <signal.h>

#include "DriveProxy.hpp"

using namespace cv;

Mat src;

GstFlowReturn appsink_new_preroll(GstAppSink *sink, gpointer user_data)
{
  GstBuffer *buffer =  gst_app_sink_pull_preroll(sink);
  return (GST_FLOW_OK);
}

GstFlowReturn appsink_new_buffer(GstAppSink *sink, gpointer user_data)
{
  GstBuffer *buffer = gst_app_sink_pull_buffer(sink);

  GstCaps *caps = gst_buffer_get_caps(buffer);
  GstStructure *structure = gst_caps_get_structure(caps, 0);
 
  gint width = 0, height = 0;
  gst_structure_get_int(structure, "width", &width);
  gst_structure_get_int(structure, "height", &height);

  unsigned char* data = GST_BUFFER_MALLOCDATA(buffer);

  src.create(height, width, CV_8UC3);

  for (int y = 0; y < height; ++y) {
    unsigned char* row = src.ptr(y);
    for (int x = 0; x < width; ++x) {
      row[x * 3 + 0] = data[(y * 3) * width + (x * 3) + 2];
      row[x * 3 + 1] = data[(y * 3) * width + (x * 3) + 1];
      row[x * 3 + 2] = data[(y * 3) * width + (x * 3) + 0];
    }
  }  

  gst_caps_unref(caps);
  gst_buffer_unref(buffer);
  
  return (GST_FLOW_OK);
}

bool isCollapsed(Vec4i& line1, Vec4i& line2, double threshold=10.0);
double lineSpace(Vec4i& line1, Vec4i& line2);
double distance(double x1, double y1, double x2, double y2);

DriveProxy	*proxy;

void	catchSig(int signum)
{
  proxy->stop();
  exit(0);
}

int main(int ac, char **av)
{
}

bool isCollapsed(Vec4i& line1, Vec4i& line2, double threshold)
{
  if (std::abs(line1[0] - line2[0]) < threshold
      && std::abs(line1[1] - line2[1]) < threshold
      && std::abs(line1[2] - line2[2]) < threshold
      && std::abs(line1[3] - line2[3]) < threshold
      )
    return (true);
  return (false);
}

double lineSpace(Vec4i& line1, Vec4i& line2)
{
  return (std::abs(line1[0] - line2[0])
	  + std::abs(line1[1] - line2[1])
	  + std::abs(line1[2] - line2[2])
	  + std::abs(line1[3] - line2[3]));
}

double distance(double x1, double y1, double x2, double y2)
{
  return (sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)));
}
