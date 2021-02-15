#include "pixiretro/app.h"
#include "PlayState.h"


class DonkeyKong final : public pxr::App
{
public:
  static constexpr int versionMajor {0};
  static constexpr int versionMinor {1};
  static constexpr const char* name {"Donkey Kong 1981 Arcade"};

  DonkeyKong();
  ~DonkeyKong();

  bool onInit();

  void onShutdown();

  std::string() getName() const {return name;}
  int getVersionMajor() const {return versionMajor;}
  int getVersionMinor() const {return versionMinor;}

private:
};

