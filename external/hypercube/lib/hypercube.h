#ifndef HYPERCUBE_H
#define HYPERCUBE_H 1
#include <axis.h>
#include <memory>
#include <sstream>
#include <vector>
namespace SEP {
class hypercube {
 public:
  hypercube() {}
  hypercube(const hypercube &hyper);
  hypercube &operator=(const hypercube &t) {
    axes = t.getAxes();
    n123 = t.getN123();
    return *this;
  }
  hypercube(const int ndim);
  hypercube(const std::shared_ptr<hypercube> hyper);
  hypercube(const SEP::axis &a1) {
    std::vector<SEP::axis> as;
    as.push_back(a1);
    setAxes(as);
  }
  hypercube(const SEP::axis &a1, const SEP::axis &a2) {
    std::vector<SEP::axis> as;
    as.push_back(a1);
    as.push_back(a2);
    setAxes(as);
  }
  hypercube(const SEP::axis &a1, const SEP::axis &a2, const SEP::axis &a3) {
    std::vector<SEP::axis> as;
    as.push_back(a1);
    as.push_back(a2);
    as.push_back(a3);
    setAxes(as);
  }
  std::shared_ptr <hypercube> clone() const{
   std::shared_ptr<hypercube> a (new hypercube(*this));
   return a;
 }
  hypercube(const SEP::axis &a1, const SEP::axis &a2, const SEP::axis &a3,
            const SEP::axis &a4) {
    std::vector<SEP::axis> as;
    as.push_back(a1);
    as.push_back(a2);
    as.push_back(a3);
    as.push_back(a4);
    setAxes(as);
  }
  hypercube(const std::vector<SEP::axis> &axes);
  void setAxes(const std::vector<SEP::axis> &axes);
  void setAxis(const int idim, const SEP::axis &ax);
  SEP::axis getAxis(const int idim) const;
  long long getN123() const { return n123; }
  void infoStream(std::stringstream &x);
  std::vector<int> getNs() const;
  void shrinkDimension(const int nmax) { axes.resize(nmax); }

  void addAxis(axis &a) { axes.push_back(a); }
  void deallocate() { axes.clear(); }
  ~hypercube() {
    this->deallocate();
  }

  void initNd(const std::vector<SEP::axis> &axes);
  std::vector<SEP::axis> returnAxes(const int nmax) const;
  int getNdim() const { return (int)axes.size(); }
  int getNdimG1() const;
  std::vector<SEP::axis> getAxes() const;
  std::vector<SEP::axis> getAxes(const int nmin) const;
  bool sameSize(const hypercube &other) const;
  bool sameSize(const std::shared_ptr<hypercube> &other) const;

 protected:
  long long n123;
  std::vector<SEP::axis> axes;
};
}  // namespace SEP
#endif
