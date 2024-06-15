#include <DS18B20.h>

class Temperature {
 public:
  void init(int pin);
  float getTemp();

 private:
  DS18B20 _ds;
};

