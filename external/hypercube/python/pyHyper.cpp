#include <pybind11/pybind11.h>
#include "hypercube.h"
namespace py = pybind11;
namespace SEP {

PYBIND11_MODULE(pyHypercube, clsHyper) {
  py::class_<axis>(clsHyper, "axis")  //
      .def(py::init<>(), "Initlialize an empty axis")
      .def(py::init<const int>(), "initialize an axis by only its size")
      .def(py::init<const int, const float, const float>(),
           "Initialize by number, origin, sampling")
      .def(py::init<const int, const float, const float, const std::string &>(),
           "Initialize by number, origin,sampling, label")
      .def(py::init<const int, const float, const float, const std::string &,
                    const std::string &>(),
           "Initialize by number, origin, sampling, label, and unit")
      .def("same_axis", (bool (axis::*)(const axis)) & axis::same_axis,
           "Check to see if two axes are the same")
      .def_readwrite("n", &axis::n)
      .def_readwrite("o", &axis::o)
      .def_readwrite("d", &axis::d)
      .def_readwrite("label", &axis::label)
      .def_readwrite("unit", &axis::unit);

  py::class_<hypercube, std::shared_ptr<hypercube>>(clsHyper, "hypercube")
      .def(py::init<>(), "Initialize with an empty hypercube")
      .def(py::init<std::shared_ptr<hypercube>>(),
           "Initialize a hypercube from a shared_ptr hypercube")
      .def(py::init<const hypercube &>(), "Initialize from another hypercube")
      .def(py::init<const std::vector<axis> &>(),
           "Initialize from a vector of arrays")
      .def(py::init<const axis &>(), "Initalize from a single axis")
      .def(py::init<const axis &, const axis &>(), "Initalize from two axes")
      .def(py::init<const int>(), "Initialize a n-d hypercube")
      .def(py::init<const axis &, const axis &, const axis &>(),
           "Initalize from three axes")
      .def(py::init<const axis &, const axis &, const axis &, const axis &>(),
           "Initalize from four axes")

      .def("setAxis",
           (void (hypercube::*)(const int, const axis &)) & hypercube::setAxis,
           "Setup a hypercube from a series of axes")
      .def("getAxis",
           (axis(hypercube::*)(const int) const) & hypercube::getAxis,
           "Get a single axis")
      .def("getAxes",
           (std::vector<axis>(hypercube::*)() const) & hypercube::getAxes,
           "Grab all axes")
      .def("getN123", (long long (hypercube::*)() const) & hypercube::getN123,
           "Grab the number of samples")
      .def("getNdim", (int (hypercube::*)() const) & hypercube::getNdim,
           "Get the number of axes")
      .def("getNdimG1", (int (hypercube::*)() const) & hypercube::getNdimG1,
           "Get the number of axes of length greater than 1")
      .def("sameSize",
           (bool (hypercube::*)(const hypercube &) const) & hypercube::sameSize,
           "Check to see if hypercube is the same size")
      .def("sameSize",
           (bool (hypercube::*)(const std::shared_ptr<hypercube> &) const) &
               hypercube::sameSize,
           "Check to see if hypercube is the same size");
}
}  // namespace SEP
