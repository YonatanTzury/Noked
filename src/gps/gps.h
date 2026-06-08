#include <TinyGPSPlus.h>

struct Location {
  double lat;
  double lon;
};

#define MAX_VALID_TIMEOUT 1500

class GPS {
public:
  bool init(int rx, int tx);
  void stop();
  bool update();
  bool getLocation(Location* out);
  bool getAltitude(double* alt);
  double getTime();
  float getSatellites();

private:
  double baseTime;
  double lastUpdated = -1;
  TinyGPSPlus gps;

  bool rawGetTime(double* out);
};
