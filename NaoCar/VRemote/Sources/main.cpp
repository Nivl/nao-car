//
// Remote.cpp
// NaoCar Remote
//

#include <QApplication>
#include <QPluginLoader>
#include <QImageReader>
#include <QLibraryInfo>
#include "Remote.hpp"
#include <QDebug>
#include <OVR.h>

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  Remote remote;

  return (remote.exec());
}
