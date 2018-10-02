
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
  mask.draw_polyline(vertices, 255);
  mask.fill({265, 163}, 255);
  auto const b = mask.boundary(255);
  for (auto p : b) {
    mask(p) = 127;
  }
  mask.write("mask.pgm");
  return 0;
}

