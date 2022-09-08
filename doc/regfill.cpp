/// \file       regfill.cpp
/// \copyright  2018-2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Program that fills region of test-image in various ways.

#include "regfill/image.hpp"
#include <random>

using namespace regfill;
using namespace std;


static vector<coords> vertices{
      {240, 155},
      {245, 164},
      {255, 170},
      {270, 173},
      {285, 172},
      {290, 166},
      {280, 161},
      {270, 155},
      {250, 153},
      {240, 155}};


static coords const zoom_tl{230, 140};
static coords const zoom_br{300, 180};


int main() {
  for(unsigned i= 0; i < vertices.size(); ++i) {
    // Correct for difference between Matlab index and C offset.
    vertices[i].col-= 1;
    vertices[i].row-= 1;
  }
  image trees("trees-raw.pgm");
  image s_mask(trees.size().cols(), trees.size().rows());
  image t_mask(trees.size().cols(), trees.size().rows());
  s_mask.draw_polyline(vertices, 1.0);
  s_mask.fill({265, 163}, 1.0);
  vector<coords> circ_verts;
  coords const   circ_cen{100, 100};
  uint16_t const circ_rad= 10;
  enum { N= 32 };
  for(unsigned i= 0; i < N; ++i) {
    double const   a  = 2.0 * i * M_PI / N;
    uint16_t const col= circ_cen.col + circ_rad * cos(a);
    uint16_t const row= circ_cen.row + circ_rad * sin(a);
    circ_verts.push_back({col, row});
  }
  s_mask.draw_polyline(circ_verts, 1.0);
  s_mask.fill(circ_cen, 1.0);
  image trees_mod1= trees;
  trees_mod1.laplacian_fill(s_mask);
  trees_mod1.write("trees-mod1.pgm");
  auto const s= s_mask.threshold();
  auto const b= s_mask.boundary();
  for(auto p: s) { t_mask(p.crd)= 1.0; }
  for(auto p: b) { t_mask(p.crd)= 1.0; }
  s_mask.write("s_mask.pgm");
  t_mask.write("t_mask.pgm");
  image trees_mod2= trees;
  trees_mod2.laplacian_fill(t_mask);
  trees_mod2.write("trees-mod2.pgm");
  image noise(trees.size().cols(), trees.size().rows());
  for(auto p: b) noise(p.crd)= fabs(trees_mod2(p.crd) - trees_mod1(p.crd));
  noise.laplacian_fill(s_mask);
  noise.write("noise.pgm");
  image                      trees_mod3= trees;
  std::default_random_engine generator;
  for(auto p: s) {
    std::normal_distribution<double> d(0.0, noise(p.crd));
    trees_mod3(p.crd)= trees_mod1(p.crd) + d(generator);
  }
  trees_mod3.write("trees-mod3.pgm");
  return 0;
}


// EOF
