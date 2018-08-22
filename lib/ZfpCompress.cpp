
#include "Zfpcompress.h"
#include <cassert>
#include <iostream>
using namespace SEP::IO;
ZfpCompression::ZfpCompression(const dataType typ, zfpMethod meth, float rate,
                               float tolerance, int precision) {
  setDataType(typ);
  _rate = rate;
  _meth = meth;
  _tolerance = tolerance;
  _precision = precision;

  setGlobalZfp();
}
void ZfpCompression::setGlobalZfp() {
  switch (_typ) {
    case IO_FLOAT:
      _ztype = zfp_type_float;
      break;
    case IO_INT:
      _ztype = zfp_type_int32;
      break;
    case IO_DOUBLE:
      _ztype = zfp_type_double;
      break;
    case IO_BYTE:
      _ztype = zfp_type_none;
      break;
    case IO_COMPLEX:
      std::cerr << "Not supporting complex yet" << std::endl;
      assert(1 == 2);
      _ztype = zfp_type_float;
      break;
    default:
      std::cerr << "Unknown type" << std::endl;
      assert(1 == 2);
  }
}

std::shared_ptr<storeBase> ZfpCompression::decompressData(
    const dataType typ, const std::vector<int> ns,
    const std::shared_ptr<storeBase> buf) {
  if (_typ == IO_BYTE) return buf;

  int ndim = 0;
  long long n123 = 1;
  for (int i = 0; i < ns.size(); i++) {
    if (ns[i] > 1) ndim = i + 1;
    n123 *= ns[i];
  }
  assert(ndim <= 3);
  zfp_stream* zfp = zfp_stream_open(NULL);
  zfp_field* field = zfp_field_alloc();
  bitstream* stream = stream_open(buf->getPtr(), buf->getSize());
  zfp_stream_set_bit_stream(zfp, stream);
  zfp_stream_rewind(zfp);
  assert(zfp_read_header(zfp, field, ZFP_HEADER_FULL));
  zfp_type type = _ztype;
  size_t typesize = zfp_type_size(type);

  std::shared_ptr<storeBase> storeOut = returnStorage(typ, n123);

  zfp_field_set_pointer(field, storeOut->getPtr());

  assert(zfp_decompress(zfp, field));

  zfp_field_free(field);
  zfp_stream_close(zfp);
  stream_close(stream);
  return storeOut;
}

std::shared_ptr<storeBase> ZfpCompression::compressData(
    const std::vector<int> ns, const std::shared_ptr<storeBase> buf) {
  if (_typ == IO_BYTE) return buf;

  int ndim = 0;
  for (int i = 0; i < ns.size(); i++) {
    if (ns[i] > 1) ndim = i + 1;
  }
  assert(ndim <= 3);

  zfp_field* field = zfp_field_alloc();
  zfp_stream* zfp = zfp_stream_open(NULL);

  size_t rawsize = 0;

  zfp_field_set_type(field, _ztype);
  zfp_field_set_pointer(field, buf->getPtr());
  switch (ndim) {
    case 1:
      zfp_field_set_size_1d(field, ns[0]);
      break;
    case 2:
      zfp_field_set_size_2d(field, ns[0], ns[1]);
      break;
    case 3:
      zfp_field_set_size_3d(field, ns[0], ns[1], ns[2]);
      break;
  }

  switch (_meth) {
    case ZFP_ACCURACY:
      zfp_stream_set_accuracy(zfp, _tolerance);
      break;
    case ZFP_TOLERANCE:
      zfp_stream_set_precision(zfp, _precision);
      break;
    case ZFP_PRECISION:
      zfp_stream_set_rate(zfp, _rate, _ztype, ndim, 0);
      break;
  }
  size_t bufsize = zfp_stream_maximum_size(zfp, field);
  assert(bufsize);
  void* buffer = malloc(bufsize);
  assert(buffer);

  bitstream* stream = stream_open(buffer, bufsize);
  assert(stream);
  zfp_stream_set_bit_stream(zfp, stream);

  zfp_write_header(zfp, field, ZFP_HEADER_FULL);
  size_t zfpsize = zfp_compress(zfp, field);
  std::shared_ptr<store<unsigned char>> x(
      new store<unsigned char>(zfpsize, field));
  /* free allocated storage */
  zfp_field_free(field);
  zfp_stream_close(zfp);
  stream_close(stream);
  free(buffer);
  return x;
}

Json::Value ZfpCompression::getJsonDescription() {
  Json::Value v;
  v["compressType"] = "noCompression";
  v["dataType"] = elementString();
  v["method"] = methodString();
  return v;
}
std::string ZfpCompression::methodString() {
  if (_meth == ZFP_ACCURACY) return "ACCURACY";
  if (_meth == ZFP_TOLERANCE) return "TOLERANCE";
  if (_meth == ZFP_PRECISION) return "PRECISION";
  return "Unknown";
}