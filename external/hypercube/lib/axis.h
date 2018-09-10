#ifndef AXIS_H
#define AXIS_H 1
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>

namespace SEP {

class axis {
 public:
  axis() {}  // Default constructor does nothing
  axis(const int n, float o, float d, const std::string &label,
       const std::string &unit);
  axis(const int n, float o, float d, const std::string &label);
  axis(const int n, float o, float d);
  axis(const int n);
  axis &operator=(const axis &t) {
    n = t.n;
    o = t.o;
    d = t.d;
    label = t.label;
    unit = t.unit;
    return *this;
  }

  void infoStream(std::stringstream &x);

  bool same_axis(const axis &ax) const;
  int n;
  float o, d;
  std::string label, unit;

  ~axis() {}

 private:
  void basic_set(const int n, const float o, const float d,
                 const std::string &label, const std::string &unit);
};
}
#endif
