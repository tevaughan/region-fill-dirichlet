
#include "image.hpp"

using namespace regfill;
using namespace std;

static vector<pcoord> vertices{
    {240, 155}, {245, 164}, {255, 170}, {270, 173}, {285, 172},
    {290, 166}, {280, 161}, {270, 155}, {250, 153}, {240, 155}};

static pcoord const zoom_tl{230,140};
static pcoord const zoom_br{300,180};

int main() {
  image img("trees-raw.pgm");
  image mask(img.num_cols(), img.num_rows());
  for (unsigned i = 0; i < vertices.size(); ++i) {
    // Correct for difference between Matlab index and C offset.
    vertices[i].col -= 1;
    vertices[i].row -= 1;
  }
  mask.draw_polyline(vertices, 1);
  mask.fill({265, 163}, 1);
  img.laplacian_fill(mask);
  auto const s = mask.threshold();
  auto const b = mask.boundary();
  vector<pcoord> t = s;
  for (auto p : b) {
    mask(p) = 0.5;
    t.push_back(p);
  }
  mask.write("mask.pgm");
  img.write("trees-mod.pgm");
  return 0;
}

