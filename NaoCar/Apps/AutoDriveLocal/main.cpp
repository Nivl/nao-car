//
// test.cpp for  in /home/jochau_g//Desktop/opencv
// 
// Made by gael jochaud-du-plessix
// Login   <jochau_g@epitech.net>
// 
// Started on  Fri Sep 28 13:59:38 2012 gael jochaud-du-plessix
// Last update Wed Feb 20 19:42:05 2013 samuel olivier
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
#include "time.h"

using namespace cv;

Mat src;

GstFlowReturn appsink_new_preroll(GstAppSink *sink, gpointer)
{
  gst_app_sink_pull_preroll(sink);
  return (GST_FLOW_OK);
}

GstFlowReturn appsink_new_buffer(GstAppSink *sink, gpointer)
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

typedef enum State {
  Up,
  Down,
  Stop
} State;

typedef enum Direction {
  Right,
  Left,
  Front
} Direction;

DriveProxy	*proxy;

#define VALUE_RIGHT 0.55
#define VALUE_LEFT 0.45
#define VALUE_TOO_RIGHT 0.85
#define VALUE_TOO_LEFT 0.15

void	getNextState(State &state, Direction &direction, double value)
{  
  static bool isStart = false;
  static clock_t start;
  
  if (state == Stop)
    state = Up;
  else if (state == Up)
    {
      if (direction == Right)
	{
	  if (value < VALUE_RIGHT && value > VALUE_LEFT)
	    direction = Front;
	  if (value <= VALUE_LEFT || value >= VALUE_TOO_RIGHT)
	    {
	      direction = Left;
	      state = Down;
	    }
	}
      else if (direction == Left)
	{
          if (value < VALUE_RIGHT && value > VALUE_LEFT)
            direction = Front;
          if (value <= VALUE_TOO_LEFT || value >= VALUE_RIGHT)
            {
	      direction = Right;
              state = Down;
            }
        }
      else
	{
	  if (value > VALUE_RIGHT)
	    direction = Right;
	  else if (value < VALUE_LEFT)
	    direction = Left;
	}
    }
  else
    {
      if (!isStart)
	{
	  proxy->stopPush();
	  isStart = true;
	  start = clock();
	}
      else
	{
	  clock_t end = clock();
	  if (((double)end - start) / CLOCKS_PER_SEC > 1.0)
	    {
	      isStart = false;
	      proxy->stopPush();
	      direction = Front;
	      state = Up;
	    }
	}
    }
}

void	catchSig(int)
{
  proxy->stop();
  exit(0);
}

int main(int, char **av)
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

  proxy = new DriveProxy(broker);
  proxy->start();
  proxy->takeSteeringWheel();
  signal(SIGINT, catchSig);
  // Create the gstreamer pipeline
  gst_init(0, NULL);
  GError* error = NULL;
  // GstElement *pipeline = gst_parse_launch("v4l2src ! video/x-raw-yuv,width=640,height=480 ! ffmpegcolorspace ! video/x-raw-rgb ! appsink", &error);
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

  int tolerance = 166;//223;
  int tolerance2 = 110;//223;
  createTrackbar("Tolerance", "opencv", &tolerance, 255);
  int untolerance = 218;//171;
  int untolerance2 = 141;//171;
  createTrackbar("Untolerance", "opencv", &untolerance, 255);
  createTrackbar("Tolerance2", "opencv", &tolerance2, 255);
  createTrackbar("Untolerance2", "opencv", &untolerance2, 255);
  int v1 = 4, v2 = 80, v3 = 80, v4 = 37, v5 = 9;
  createTrackbar("v1", "opencv", &v1, 200);
  createTrackbar("v2", "opencv", &v2, 200);
  createTrackbar("v3", "opencv", &v3, 200);
  createTrackbar("v4", "opencv", &v4, 200);
  createTrackbar("v5", "opencv", &v5, 200);

  Mat mask;
  Vec4i redLine(0, 0, 0, 0);

  double objectivesStartY = 150 * src.rows / 480;
  double objectivesEndY = src.rows;
  double objectivesStep = 1;
  
  vector<Vec2i> points;
  for (size_t k = 0; k < (objectivesEndY - objectivesStartY) / objectivesStep; ++k)
    {
      Vec2i point(320 * src.cols / 640, objectivesEndY - objectivesStep * k);
      points.push_back(point);
    }

  //int previousStatus = 0;
  //int previousMotion = 0;
  State state = Stop;
  Direction direction = Front;
  double values[5] = {-1, -1, -1, -1, -1};
  do {

    if (!src.data)
      continue ;

    Mat hsv;
    cvtColor(src, hsv, CV_RGB2HSV);
    Mat color_dst = hsv;
    inRange(hsv,
    	    Scalar(tolerance2, tolerance, 0),
    	    Scalar(untolerance2, untolerance, 255),
    	    mask);
    // inRange(src,
    // 	    Scalar(40, 40, 40),
    // 	    Scalar(122, 122, 122),
    // 	    mask);



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
	  if (isCollapsed(lines[i], collapsedLines[j], 50 * src.rows / 640)) {
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

    // std::cout << lines.size() << " lines found, collapsed in " << collapsedLines.size() << std::endl;

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
	// std::cout << "Choosed nearest\n";
	if (std::abs(nearest[1] - nearest[3]) > 50 * src.cols / 640) {
	  redLine = nearest;
	  found = true;
	}
    }
    // If we don't have a line, take the first of the list
    if (!found && collapsedLines.size() > 0) {
      // std::cout << "Choosed first\n";
      if (std::abs(collapsedLines[0][1] - collapsedLines[0][3]) > 50 * src.cols / 640)
	redLine = collapsedLines[0];
    }
    // Find objectives points
    size_t index = 0;

    for (size_t y = objectivesEndY; y > objectivesStartY; y -= objectivesStep)
      {
	double prevX = 320 * src.cols / 640;
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
		  if  (distanceToPrevX == -1 || abs(x - prevX) + abs(x -  320 * src.cols / 640) < distanceToPrevX)
		    {
		      distanceToPrevX = abs(x - prevX) + abs(x - 320 * src.cols / 640);
		      newX = x;
		    }
		}
	    }
	}
	points[index][0] = newX;
	// if (newX != -1)
	//   {
	//     if (points[index][0] - newX > 5)
	//       points[index][0] -= 5;
	//     else if (points[index][0] - newX < -5)
	//       points[index][0] += 5;
	//     //else
	//     points[index][0] = newX;
	//   }
	// else if (index > 1)
	//   {
	//     if (points[index - 1][1] != collapsedLines[index - 2][1] && 0)
	//       {
	// 	double dx = (double)(points[index - 1][0] - points[index - 2][0]) / (points[index - 1][1] - collapsedLines[index - 2][1]);
	// 	Vec2i point(points[index - 1][0] + dx * objectivesStep, y);
	// 	points[index] = point;
	//       }
	//   }
	index += 1;
      }

    //Debug lines
    for (int i = 0; i < collapsedLines.size(); ++i) {
      line(color_dst, Point(collapsedLines[i][0], collapsedLines[i][1]),
    	   Point(collapsedLines[i][2], collapsedLines[i][3]), Scalar(0, 0, 255), 3, 8);
    }

    // LA OU CA MARCHE PAS MAIS IL FAUT LENLEVER ET LA FAIRE MARCHER AVANT, CA DOIT AFFICHER UNE PETITE LIGNE ROUGE(COMME TA BITE)
    // line(color_dst, Point(0, 0), Point(50, 50), Scalar(255, 0, 0), 3, 8);
    // for (int i = 0; i < points.size() - 1; ++i) {
    //   line(color_dst, points[i],
    // 	   points[i + 1], Scalar(255, 255, 0), 3, 8);
    // }

    //line(color_dst, Point(redLine[0], redLine[1]),
    //	 Point(redLine[2], redLine[3]), Scalar(0, 255, 0), 3, 8);

    vector<double> roadCoords;
    for (int value = 380; value < 440; value += 15)
      roadCoords.push_back(value * src.rows / 480);
    // roadCoords.push_back(180 * src.rows / 480);
    // roadCoords.push_back(200 * src.rows / 480);
    // roadCoords.push_back(225 * src.rows / 480);
    // roadCoords.push_back(250 * src.rows / 480);
    // roadCoords.push_back(280 * src.rows / 480);
    // roadCoords.push_back(310 * src.rows / 480);
    // roadCoords.push_back(340 * src.rows / 480);
    // roadCoords.push_back(370 * src.rows / 480);
    // roadCoords.push_back(400 * src.rows / 480);
    vector<Vec2i> road;
    for (int i = 0; i < points.size() - 1; ++i) {
      for (int k = 0; k < roadCoords.size(); ++k)
	{
	  if (points[i][1] == roadCoords[k] && points[i][0] != -1)
	    {
	      road.push_back(points[i]);
	    }
	}
    }

    for (int i = 0; road.size() != 0 && i < road.size() - 1; ++i) {
      line(color_dst, road[i],
           road[i + 1], Scalar(255, 0, 0), 3, 8);
      line(color_dst, Point(0, road[i][1]),
           Point(src.cols, road[i][1]), Scalar(0, 255 - i * 10, 0), 1, 1);
    }
    if (road.size() != 0)
      {
	line(color_dst, Point(0, road[road.size() - 1][1]),
	     Point(src.cols, road[road.size() - 1][1]), Scalar(0, 255 - (road.size() - 1) * 10, 0), 1, 1);
      }
    double sum = 0.0;
    double div = 0.0;
    for (int i = 0; road.size() != 0 && i < road.size(); ++i) {
      sum += (road.size() - i) * road[i][0];
      div += (road.size() - i);
    }
    if (div > 0)
      sum /= div;

    for (int i = 5; i > 0; --i)
      values[i] = values[i - 1];
    values[0] = sum / src.cols;
    double total = 0;
    div = 0;
    for (int i = 0; i < 5; ++i)
      if (values[i] > 0)
    	{
	  std::cout << values[i] << std::endl;
    	  total += values[i];
    	  div += 1;
    	}
    if (div >= 1)
      total /= div;

    if (total > 0)
      getNextState(state, direction, total);
    std::cout << "TOTAL " << total << std::endl;
    std::cout << state << std::endl;
    std::cout << direction << std::endl;

    if (direction == Right)
      proxy->right();
    else if (direction == Left)
      proxy->left();
    else
      proxy->stopTurn();
    if (state == Up)
      proxy->up();
    else if (state == Down)
      proxy->down();
    else
      proxy->stopPush();

    // bool recule = false;
    // if (sum == 0) {}
    // else if (sum > 450 * src.cols / 640 && (previousStatus == 1 || previousStatus == 0))
    //   {
    // 	proxy->left();
    // 	std::cout << "LEFT" << std::endl;
    // 	recule = true;
    //   }
    // else if (sum < 190 * src.cols / 640 && (previousStatus == 2 || previousStatus == 0))
    //   {
    // 	proxy->right();
    // 	std::cout << "RIGHT" << std::endl;
    // 	recule = true;
    //   }
    // else if (sum > 360 * src.cols / 640 && (previousStatus == 1 || previousStatus == 0))
    //   {
    // 	proxy->right();
    // 	std::cout << "RIGHT" << std::endl;
    // 	previousStatus = 1;
    //   }
    // else if (sum < 280 * src.cols / 640 && (previousStatus == 2 || previousStatus == 0))
    //   {
    // 	proxy->left();
    // 	std::cout << "LEFT" << std::endl;
    // 	previousStatus = 2;
    //   }
    // else if (sum < 360 * src.cols / 640 && sum > 280 * src.cols / 640 )
    //   {
    // 	proxy->stopTurn();
    // 	std::cout << "FRONT" << std::endl;
    // 	previousStatus = 0;
    //   }
    // else
    //   {
    // 	if (previousStatus == 1)
    // 	  {
    // 	    proxy->left();
    // 	    recule = true;
    // 	    previousStatus = 0;
    // 	  }
    // 	if (previousStatus == 2)
    // 	  {
    // 	    proxy->right();
    // 	    recule = true;
    // 	    previousStatus = 0;
    // 	  }
    // 	else
    // 	  proxy->stopTurn();
    //   }
    // if ((recule && (previousMotion >= 0 || previousMotion == -100)) || (previousMotion >= 1 && previousMotion != 100))
    //   {
    // 	std::cout << "RECULE" << std::endl;
    // 	if (previousMotion > 90)
    // 	  previousMotion = 100;
    // 	else if (previousMotion <= 0)
    // 	  {
    // 	    proxy->stopPush();
    // 	    previousMotion = 1;
    // 	  }
    // 	else
    // 	  ++previousMotion;
    // 	//proxy->down();
    //   }
    // else
    //   {
    // 	std::cout << "TOUT DROIT" << std::endl;
    // 	if (previousMotion < -60)
    // 	  previousMotion = -100;
    // 	else if (previousMotion >= 0)
    // 	  {
    // 	    proxy->stopPush();
    // 	    previousMotion = -1;
    // 	  }
    // 	else
    // 	  --previousMotion;
    // 	//proxy->up();
    //   }
    if (total > 0)
      line(color_dst, Point(total * src.cols, 0),
	   Point(total * src.cols, src.rows), Scalar(255, 255, 255), 3, 8);
    imshow("opencv", color_dst);
    imshow("opencv2", mask);
  } while (waitKey(10) != 'q');

  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(GST_OBJECT(pipeline));

  proxy->stop();
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
