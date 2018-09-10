#include "Zfpcompress.h"
#include <cassert>
#include <iostream>
#include "ioTypes.h"
using namespace SEP::IO;
ZfpCompression::ZfpCompression(const SEP::dataType typ, const ZfpParams pars) {
  setDataType(typ);
  _rate = pars._rate;
  _meth = pars._meth;
  _tolerance = pars._tolerance;
  _precision = pars._precision;
  _typ = typ;

  setGlobalZfp();
}
ZfpCompression::ZfpCompression(const Json::Value& des) {
  _rate = des["rate"].asFloat();
  _tolerance = des["tolerance"].asFloat();
  _precision = des["precision"].asInt();

  setDataType(toElementType(des["dataType"].asString()));

  stringToMethod(des["method"].asString());
}

void ZfpCompression::setGlobalZfp() {
  switch (_typ) {
    case DATA_FLOAT:
      _ztype = zfp_type_float;
      break;
    case DATA_INT:
      _ztype = zfp_type_int32;
      break;
    case DATA_DOUBLE:
      _ztype = zfp_type_double;
      break;
    case DATA_BYTE:
      _ztype = zfp_type_none;
      break;
    case DATA_COMPLEX:
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
    const std::vector<int> ns, const std::shared_ptr<storeBase> buf) {
  if (_typ == DATA_BYTE) return buf;

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

  std::shared_ptr<storeBase> storeOut = returnStorage(_typ, n123);

  zfp_field_set_pointer(field, storeOut->getPtr());

  assert(zfp_decompress(zfp, field));

  zfp_field_free(field);
  zfp_stream_close(zfp);
  stream_close(stream);
  return storeOut;
}

std::shared_ptr<storeBase> ZfpCompression::compressData(
    const std::vector<int> ns, const std::shared_ptr<storeBase> buf) {
  if (_typ == DATA_BYTE) return buf;

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
    case ZFP_PRECISION:
      zfp_stream_set_precision(zfp, _precision);
      break;
    case ZFP_RATE:

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

  assert(zfp_write_header(zfp, field, ZFP_HEADER_FULL));

  size_t zfpsize = zfp_compress(zfp, field);

  std::shared_ptr<storeByte> x(new storeByte(zfpsize, buffer));

  /* free allocated storage */
  zfp_field_free(field);

  zfp_stream_close(zfp);

  stream_close(stream);

  free(buffer);
  return x;
}

Json::Value ZfpCompression::getJsonDescription() {
  Json::Value v;
  v["compressType"] = "ZfpCompression";
  v["dataType"] = elementString();
  v["method"] = methodToString();
  v["tolerance"] = _tolerance;
  v["rate"] = _rate;
  v["precision"] = _precision;

  return v;
}
std::string ZfpCompression::methodToString() {
  if (_meth == ZFP_ACCURACY) return "ACCURACY";
  if (_meth == ZFP_RATE) return "RATE";
  if (_meth == ZFP_PRECISION) return "PRECISION";
  return "Unknown";
}
void ZfpCompression::stringToMethod(const std::string& meth) {
  if (meth == "ACCURACY") ZFP_ACCURACY;
  if (meth == "RATE") ZFP_RATE;
  if (meth == "PRECISION") ZFP_PRECISION;
}
