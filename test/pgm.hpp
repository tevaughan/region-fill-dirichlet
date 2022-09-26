/// \file       test/pgm.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of test::pgm::read(), test::pgm::write().

#ifndef TEST_PGM_HPP
#define TEST_PGM_HPP

#include "drawMask.hpp" // Image
#include <fstream>      // ifstream

namespace test::pgm {


using std::ifstream;
using std::istream;
using std::ofstream;
using std::string;


struct PgmHeader {
  string magic;
  int    width;
  int    height;
  int    maxval;

  PgmHeader(istream &is) {
    if(!(is >> magic)) throw "PgmHeader: reading magic";
    if(magic != "P5") throw "PgmHeader: bad magic";
    if(!(is >> width)) throw "PgmHeader: reading width";
    if(!(is >> height)) throw "PgmHeader: reading height";
    if(!(is >> maxval)) throw "PgmHeader: reading maxval";
  }

  int numPix() const { return width * height; }
};


inline Image read(char const *f) {
  ifstream        ifs(f);
  PgmHeader const pgmHeader(ifs);
  ArrayXXi        img(pgmHeader.height, pgmHeader.width);
  int             n     = pgmHeader.numPix();
  int             i     = 0;
  bool            inData= false;
  while(i < n && ifs) {
    int const v= ifs.get();
    if(inData == false && (v == ' ' || v == '\t' || v == '\n')) continue;
    inData     = true;
    int const r= i / pgmHeader.width;
    int const c= i % pgmHeader.width;
    img(r, c)  = v;
    ++i;
  }
  return img;
}


inline void write(char const *f, Image const &image) {
  ofstream ofs(f);
  if(!(ofs << "P5\n")) throw "writePgm: magic";
  if(!(ofs << image.cols() << " " << image.rows() << "\n")) {
    throw "writePgm::size";
  }
  Image im = image;
  int   min= im.minCoeff();
  if(min < 0) {
    im-= min;
    min= 0;
  }
  int max= im.maxCoeff();
  if(max > 255) {
    im = (im - min) * 255 / (max - min);
    max= 255;
  }
  if(!(ofs << max << "\n")) throw "writePgm: max";
  for(int r= 0; r < image.rows(); ++r) {
    for(int c= 0; c < image.cols(); ++c) ofs.put(image(r, c));
  }
}


} // namespace test::pgm

#endif // ndef TEST_PGM_HPP

// EOF
