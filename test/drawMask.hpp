/// \file       test/drawMask.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of test::Image, test::drawMask().

#ifndef TEST_DRAW_MASK_HPP
#define TEST_DRAW_MASK_HPP

#include <eigen3/Eigen/Dense> // Array, ArrayXXi, Dynamic, RowMajor

namespace test {


using Eigen::Array;
using Eigen::ArrayXi;
using Eigen::ArrayXXi;
using Eigen::Dynamic;
using Eigen::RowMajor;
using std::min_element;


using Image= Array<int, Dynamic, Dynamic, RowMajor>;


/// Try to draw circle of radius `radiusPix` pixels at center of image that is
/// same size as `image`, and also draw thin, vertical line at center.
inline Image drawMask(Image const &image, int radiusPix= 100, int w= 10) {
  int const rcen  = image.rows() / 2;
  int const ccen  = image.cols() / 2;
  int const rads[]= {radiusPix, int(image.rows()), int(image.cols())};
  int const rpix  = *min_element(rads, rads + 3);
  int const r2    = rpix * rpix;
  ArrayXXi  col(1, image.cols());
  ArrayXXi  row(image.rows(), 1);
  col.row(0)       = ArrayXi::LinSpaced(image.cols(), 0, image.cols() - 1);
  row.col(0)       = ArrayXi::LinSpaced(image.rows(), 0, image.rows() - 1);
  ArrayXXi cols    = col.replicate(image.rows(), 1);
  ArrayXXi rows    = row.replicate(1, image.cols());
  ArrayXXi dr      = rows - rcen;
  ArrayXXi dc      = cols - ccen;
  ArrayXXi inCircle= (dr * dr + dc * dc < r2).cast<int>();
  ArrayXXi inLine= (cols >= ccen - w / 2 && cols <= ccen + w / 2).cast<int>();
  return inCircle + inLine;
}


} // namespace test

#endif // ndef TEST_DRAW_MASK_HPP

// EOF

