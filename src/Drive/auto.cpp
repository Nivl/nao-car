//
// test.cpp for  in /home/jochau_g//Desktop/opencv
// 
// Made by gael jochaud-du-plessix
// Login   <jochau_g@epitech.net>
// 
// Started on  Fri Sep 28 13:59:38 2012 gael jochaud-du-plessix
// Last update Tue Oct  2 15:05:16 2012 loick michard
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

int main(int ac, char **av)
{
  boost::shared_ptr<AL::ALBroker> broker;
  try
    {
      broker = AL::ALBroker::createBroker("broker","0.0.0.0",0,av[1],9559,0);
    }
  catch (...)
    {
      std::cerr << "Error" << std::endl;
      return (1);
    }
  

  // Create the gstreamer pipeline
  gst_init(0, NULL);
  GError* error = NULL;
  //GstElement *pipeline = gst_parse_launch("v4l2src ! video/x-raw-yuv,width=640,height=480 ! ffmpegcolorspace ! video/x-raw-rgb ! appsink", &error);
  GstElement *pipeline = gst_parse_launch("udpsrc port=8081 ! smokedec ! ffmpegcolorspace ! video/x-raw-rgb ! appsink", &error);

  if (!pipeline || error) {
    std::cerr << "Cannot create pipeline" << std::endl;
    return (1);
  }

  GstElement* appsink = gst_bin_get_by_name(GST_BIN(pipeline), "appsink0");

  // Register callbacks                                                                                                                                     
  GstAppSinkCallbacks callbacks = {
    NULL, appsink_new_preroll, appsink_new_buffer,
    NULL, { NULL }
  };
  gst_app_sink_set_callbacks(GST_APP_SINK(appsink), &callbacks, NULL, NULL);

  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  // Create the window
  namedWindow("opencv", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
  cvMoveWindow("opencv", 0, 0);
  namedWindow("opencv2", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
  cvMoveWindow("opencv2", 640, 0);

  int tolerance = 181;
  createTrackbar("Tolerance", "opencv", &tolerance, 255);
  int untolerance = 98;
  createTrackbar("Untolerance", "opencv", &untolerance, 255);
  int v1 = 4, v2 = 80, v3 = 80, v4 = 30, v5 = 50;
  createTrackbar("v1", "opencv", &v1, 200);
  createTrackbar("v2", "opencv", &v2, 200);
  createTrackbar("v3", "opencv", &v3, 200);
  createTrackbar("v4", "opencv", &v4, 200);
  createTrackbar("v5", "opencv", &v5, 200);

  Mat mask;
  Vec4i redLine(0, 0, 0, 0);

  double objectivesStartY = 200;
  double objectivesEndY = 480;
  double objectivesStep = 2;
  
  vector<Vec2i> points;
  for (size_t k = 0; k < (objectivesEndY - objectivesStartY) / objectivesStep; ++k)
    {
      Vec2i point(320, objectivesEndY - objectivesStep * k);
      points.push_back(point);
    }

  do {
    if (!src.data)
      continue ;

    // inRange(src,
    // 	    Scalar(0, 0, tolerance),
    // 	    Scalar(untolerance, untolerance, 255),
    // 	    mask);
    inRange(src,
	    Scalar(40, 40, 40),
	    Scalar(122, 122, 122),
	    mask);

    Mat color_dst = src;

    vector<Vec4i> lines;
    HoughLinesP(mask, lines, v1, CV_PI / 180, v3, v4, v5);
    // First assure all lines are oriented upward
    for(size_t i = 0; i < lines.size(); i++ ) {
      if (lines[i][1] < lines[i][3]) {
	Vec4i tmp = lines[i];
	lines[i][0] = tmp[2];
	lines[i][1] = tmp[3];
	lines[i][2] = tmp[0];
	lines[i][3] = tmp[1];
      }
    }

    vector<Vec4i> collapsedLines;
    // Start by collapse together lines near to each others
    for(size_t i = 0; i < lines.size(); i++ ) {
      if (collapsedLines.size() < 1)
	collapsedLines.push_back(lines[i]);
      else {
	bool found = false;
	if (1){
	for (size_t j = 0; j < collapsedLines.size() && !found; ++j) {
	  if (isCollapsed(lines[i], collapsedLines[j], 50)) {
	    double max = -1;
	    for (int k = 0; k < 4; k++)
	      {
		double dist = distance(collapsedLines[j][0] * (k % 2) + lines[i][0] * ((k + 1) % 2),
				       collapsedLines[j][1] * (k % 2) + lines[i][1] * ((k + 1) % 2),
				       collapsedLines[j][2] * (k / 2) + lines[i][2] * ((3 - k) / 2),
				       collapsedLines[j][3] * (k / 2) + lines[i][3] * ((3 - k) / 2));
		if (dist > max)
		  {
		    max = dist;
		    collapsedLines[j][0] = collapsedLines[j][0] * (k % 2) + lines[i][0] * ((k + 1) % 2);
		    collapsedLines[j][1] = collapsedLines[j][1] * (k % 2) + lines[i][1] * ((k + 1) % 2);
		    collapsedLines[j][2] = collapsedLines[j][2] * (k / 2) + lines[i][2] * ((3 - k) / 2);
		    collapsedLines[j][3] =  collapsedLines[j][3] * (k / 2) + lines[i][3] * ((3 - k) / 2);
		  }
	      }
	    found = true;
	  }
	}
	}
	if (!found)
	  collapsedLines.push_back(lines[i]);
      }
    }

    std::cout << lines.size() << " lines found, collapsed in " << collapsedLines.size() << std::endl;

    // Once we have collapsed lines, search for the best one
    // If we already have one, just take the nearest    
    double redLineLength = std::abs(redLine[1] - redLine[3]);
    bool found = false;
    if ((collapsedLines.size() > 0
	 && !(redLine[0] == 0 && redLine[1] == 0 && redLine[1] == 0 && redLine[1] == 0))
	&& lineSpace(collapsedLines[0], redLine) < redLineLength) {
	Vec4i nearest = collapsedLines[0];
	double minSpace = lineSpace(collapsedLines[0], redLine);
	for (int i = 1; i < collapsedLines.size(); ++i) {
	  double space = lineSpace(collapsedLines[i], redLine);
	  // If the lines are not too far from each other
	  if (space < minSpace && (space < redLineLength)) {
	    nearest = collapsedLines[i];
	    minSpace = space;
	  }
	}
	std::cout << "Choosed nearest\n";
	if (std::abs(nearest[1] - nearest[3]) > 50) {
	  redLine = nearest;
	  found = true;
	}
    }
    // If we don't have a line, take the first of the list
    if (!found && collapsedLines.size() > 0) {
      std::cout << "Choosed first\n";
      if (std::abs(collapsedLines[0][1] - collapsedLines[0][3]) > 50)
	redLine = collapsedLines[0];
    }
    // Find objectives points
    size_t index = 0;

    for (size_t y = objectivesEndY; y > objectivesStartY; y -= objectivesStep)
      {
	double prevX = 320;
	if (index > 0)
	  prevX = points[index - 1][0];
	double distanceToPrevX = -1;
	double newX = -1;
	for(size_t i = 0; i < collapsedLines.size(); i++ ) {
	  if ((collapsedLines[i][1] >= y && collapsedLines[i][3] <= y) || (collapsedLines[i][1] <= y && collapsedLines[i][3] >= y))
	    {
	      if (collapsedLines[i][2] != collapsedLines[i][0])
		{
		  double k = (double)(collapsedLines[i][3] - collapsedLines[i][1]) / (collapsedLines[i][2] - collapsedLines[i][0]);
		  double b = collapsedLines[i][3] - k * collapsedLines[i][2];
		  double x = (y - b) / k;
		  if  (distanceToPrevX == -1 || abs(x - prevX) + abs(x - 320) < distanceToPrevX)
		    {
		      distanceToPrevX = abs(x - prevX) + abs(x - 320);
		      newX = x;
		    }
		}
	    }
	}
	if (newX != -1)
	  {
	    if (points[index][0] - newX > 5)
	      points[index][0] -= 5;
	    else if (points[index][0] - newX < -5)
	      points[index][0] += 5;
	    //else
	    //points[index][0] = newX;
	  }
	else if (index > 1)
	  {
	    if (points[index - 1][1] != collapsedLines[index - 2][1] && 0)
	      {
		double dx = (double)(points[index - 1][0] - points[index - 2][0]) / (points[index - 1][1] - collapsedLines[index - 2][1]);
		Vec2i point(points[index - 1][0] + dx * objectivesStep, y);
		points[index] = point;
	      }
	  }
	index += 1;
      }

    //Debug lines
    // for (int i = 0; i < collapsedLines.size(); ++i) {
    //   line(color_dst, Point(collapsedLines[i][0], collapsedLines[i][1]),
    // 	   Point(collapsedLines[i][2], collapsedLines[i][3]), Scalar(255, 0, 0), 3, 8);
    // }

    // LA OU CA MARCHE PAS MAIS IL FAUT LENLEVER ET LA FAIRE MARCHER AVANT, CA DOIT AFFICHER UNE PETITE LIGNE ROUGE(COMME TA BITE)
    line(color_dst, Point(0, 0), Point(50, 50), Scalar(255, 0, 0), 3, 8);
    for (int i = 0; i < points.size() - 1; ++i) {
      line(color_dst, points[i],
	   points[i + 1], Scalar(255, 255, 0), 3, 8);
    }

    //line(color_dst, Point(redLine[0], redLine[1]),
    //	 Point(redLine[2], redLine[3]), Scalar(0, 255, 0), 3, 8);

    vector<double> roadCoords;
    roadCoords.push_back(280);
    roadCoords.push_back(310);
    roadCoords.push_back(350);
    roadCoords.push_back(380);
    roadCoords.push_back(400);
    vector<Vec2i> road;
    for (int i = 0; i < points.size() - 1; ++i) {
      for (int k = 0; k < roadCoords.size(); ++k)
	{
	  if (points[i][1] == roadCoords[k])
	    road.push_back(points[i]);
	}
    }

    for (int i = 0; i < road.size() - 1; ++i) {
      line(color_dst, road[i],
           road[i + 1], Scalar(255, 0, 0), 3, 8);
    }

    double sum = 0.0;
    for (int i = 0; i < road.size(); ++i) {
      sum += road[i][0];
    }
    sum /= road.size();

    // LA OU CA SEGFAULT NORMALEMENT
    std::cout << sum << std::endl;
    line(color_dst, Point(sum, 0),
    	 Point(sum, 480), Scalar(255, 255, 255), 3, 8);

    imshow("opencv", color_dst);
    imshow("opencv2", mask);
  } while (waitKey(10) != 'q');

  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(GST_OBJECT(pipeline));

  return 0;
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
