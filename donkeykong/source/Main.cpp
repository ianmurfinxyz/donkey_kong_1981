#include "pixiretro/pxr_engine.h"
#include "DonkeyKong.h"

pxr::Engine engine;

int main()
{
  engine.initialize(std::unique_ptr<pxr::App>(new DonkeyKong{}));
  engine.run();
  engine.shutdown();
}
