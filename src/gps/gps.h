struct Location
{
  double lat;
  double lon;
};

class GPS {
 public:
  void init();
  bool getLocation(Location* out);
  double getTime();

 private:
  double baseTime;
  double lastUpdated;

  bool rawGetTime(double* out);
};
