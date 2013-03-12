//
// Remote.cpp
// NaoCar Remote
//

#include "Remote.hpp"

int main(int argc, char **argv) {
  Remote remote(argc, argv);
  return (remote.exec());
}
