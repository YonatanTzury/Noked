#include <TinyGPSPlus.h>

struct Location
{
  double lat;
  double lon;
};

#define MAX_VALID_TIMEOUT 1500

class GPS {
 public:
  void init();
  bool update();
  bool getLocation(Location* out);
  bool getAltitude(double* alt);
  double getTime();

 private:
  double baseTime;
  double lastUpdated;
  TinyGPSPlus gps;

  bool rawGetTime(double* out);
};
