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
  uint32_t getTime();
  float getSatellites();

private:
  uint32_t baseTime;    // GPS time as Unix epoch seconds at the last valid fix.
  uint32_t lastUpdated; // millis() captured at the last valid fix.
  TinyGPSPlus gps;

  bool rawGetTime(uint32_t* out);
};
