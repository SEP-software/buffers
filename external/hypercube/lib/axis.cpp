#include <assert.h>
#include <axis.h>
#include <math.h>

using std::string;
using namespace SEP;

axis::axis(const int n, const float o, const float d, const string& label) {
  string unit = "Undefined";
  this->basic_set(n, o, d, label, unit);
}
axis::axis(const int n, const float o, const float d, const string& label,
           const string& unit) {
  this->basic_set(n, o, d, label, unit);
}
axis::axis(const int n, const float o, const float d) {
  string unit = "Undefined", label = "Undefined";
  this->basic_set(n, o, d, label, unit);
}
bool axis::same_axis(const axis& ax) const {
  bool match = true;
  assert(this->d > 1e-7);
  if (this->n != ax.n) match = false;
  ;
  if (fabs((this->o - ax.o) / this->d) > .01) match = false;
  if (fabs(this->d / ax.d - 1.) > .01) match = false;
  return match;
}
axis::axis(const int n) {
  string unit = "Undefined", label = "Undefined";
  this->basic_set(n, 0., 1, label, unit);
}
void axis::basic_set(const int n, const float o, const float d,
                     const string& label, const string& unit) {
  this->n = n;
  this->o = o;
  this->d = d;
  this->label = label;
  this->unit = unit;
}

void axis::infoStream(std::stringstream& x) {
  x << std::string(" n=") << std::to_string(n) << std::string(" o=")
    << std::to_string(o);
  x << std::string("d=") << std::to_string(d) << std::string(" label=")
    << label;
}
