#include <TinyGPSPlus.h>

struct Location
{
  double lat;
  double lon;
};

#define MAX_VALID_TIMEOUT 1500

class GPS {
 public:
  void init(int rx, int tx);
  bool update();
  bool getLocation(Location* out);
  bool getAltitude(double* alt);
  bool getTime(double* out);

 private:
  double baseTime;
  double lastUpdated = -1;
  TinyGPSPlus gps;

  bool rawGetTime(double* out);
};
