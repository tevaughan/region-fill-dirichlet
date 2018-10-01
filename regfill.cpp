
#include "image.hpp"

using namespace regfill;
using namespace std;

struct pcoord {
  unsigned col;
  unsigned row;
};

enum { INT_BOUND_SZ = 10 };

static array<pcoord, INT_BOUND_SZ> const internal_boundary{{{240, 155},
                                                            {245, 164},
                                                            {255, 170},
                                                            {270, 173},
                                                            {285, 172},
                                                            {290, 166},
                                                            {280, 161},
                                                            {270, 155},
                                                            {250, 153},
                                                            {240, 155}}};

static pcoord const zoom_tl{230,140};
static pcoord const zoom_br{300,180};

int main() {
  image img("trees-raw.pgm");
  return 0;
}

