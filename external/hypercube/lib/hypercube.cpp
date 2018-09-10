#include <assert.h>
#include <hypercube.h>

using namespace SEP;
hypercube::hypercube(const hypercube &hyper) {
  std::vector<axis> axes;
  axes = hyper.getAxes();
  initNd(axes);
}
hypercube::hypercube(const std::shared_ptr<hypercube> hyper) {
  std::vector<axis> axes;
  axes = hyper->getAxes();
  initNd(axes);
}
hypercube::hypercube(const int ndim) {
  std::vector<axis> axes;
  for (int i = 0; i < ndim; i++) axes.push_back(axis(1));
  initNd(axes);
}
hypercube::hypercube(const std::vector<axis> &axes) { initNd(axes); }
void hypercube::initNd(const std::vector<axis> &ax) {
  this->n123 = 1;
  for (int i = 0; i < ax.size(); i++) {
    axes.push_back(ax[i]);
    this->n123 = this->n123 * (long long)ax[i].n;
  }
}
std::vector<axis> hypercube::getAxes(const int nmin) const {
  std::vector<axis> ax = getAxes();
  for (int i = ax.size(); i < nmin; i++) ax.push_back(axis(1));
  return ax;
}
void hypercube::setAxis(const int idim, const axis &myaxis) {
  if (idim < 1 || idim > axes.size()) {
    fprintf(stderr, "IDIM=%d axes.size()=%d \n", idim, (int)axes.size());
    assert(1 == 2);
  }
  this->axes[idim - 1] = myaxis;
}
void hypercube::setAxes(const std::vector<axis> &axes) { this->initNd(axes); }
std::vector<axis> hypercube::returnAxes(const int nmax) const {
  std::vector<axis> ax;
  for (int i = 0; i < nmax; i++) {
    if (i + 1 <= this->axes.size()) {
      ax.push_back(axes[i]);
    } else {
      ax.push_back(axis(1));
    }
  }
  return ax;
}
void hypercube::infoStream(std::stringstream &x) {
  for (int i = 0; i < (int)axes.size(); i++) {
    x << "Axis " << std::to_string(i + 1);
    axes[i].infoStream(x);
    x << "\n";
  }
}
axis hypercube::getAxis(const int idim) const {
  if (idim < 1 || idim > this->axes.size()) {
    fprintf(stderr, "IDIM=%d axes.size()=%d \n", idim, (int)this->axes.size());
    assert(1 == 2);
  }
  axis myaxis = this->axes[idim - 1];
  return myaxis;
}
int hypercube::getNdimG1() const {
  int nd = this->axes.size();
  for (int i = this->axes.size() - 1; i >= 0; i--) {
    if (axes[i].n > 1) return nd;
    nd--;
  }

  return nd;
}

std::vector<axis> hypercube::getAxes() const { return returnAxes(axes.size()); }
std::vector<int> hypercube::getNs() const {
  int i;
  std::vector<int> n;
  for (i = 0; i < this->axes.size(); i++) n.push_back(axes[i].n);
  return n;
}
bool hypercube::sameSize(const hypercube &other) const {
  if (this->getNdim() != other.getNdim()) return false;
  for (int i = 0; i < this->getNdim(); i++) {
    axis ax1 = this->getAxis(i + 1);
    axis ax2 = other.getAxis(i + 1);
    if (ax1.n != ax2.n) return false;
  }
  return true;
}
bool hypercube::sameSize(const std::shared_ptr<hypercube> &other) const {
  if (this->getNdim() != other->getNdim()) return false;
  for (int i = 0; i < this->getNdim(); i++) {
    axis ax1 = this->getAxis(i + 1);
    axis ax2 = other->getAxis(i + 1);
    if (ax1.n != ax2.n) return false;
  }
  return true;
}
