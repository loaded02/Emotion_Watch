#define neoAnimFPS 30

const uint16_t PROGMEM neoAnimPixelData[] = {
  0XFA83, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000,
  0X0000, 0X0000, 0XFA83, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000,
  0X0000, 0X0000, 0X0000, 0X0000, 0XFA83, 0X0000, 0X0000, 0X0000, 0X0000,
  0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0XFA83, 0X0000, 0X0000,
  0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0XFA83,
  0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000,
  0X0000, 0XFA83, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000,
  0X0000, 0X0000, 0X0000, 0XFA83, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000,
  0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0XFA83, 0X0000, 0X0000, 0X0000,
  0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0XFA83, 0X0000,
  0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000,
  0XFA83, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000,
  0X0000, 0XFA83, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000,
  0X0000, 0X0000, 0XFA83, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000,
  0X0000, 0X0000, 0X0000, 0XFA83, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000,
  0X0000, 0X0000, 0X0000, 0X0000, 0XFA83, 0X0000, 0X0000, 0X0000, 0X0000,
  0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0XFA83, 0X0000, 0X0000, 0X0000,
  0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0XFA83, 0X0000, 0X0000,
  0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0XFA83, 0X0000,
  0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0XFA83,
  0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000,
  0XFA83, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000,
  0X0000, 0XFA83, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000,
  0X0000, 0X0000, 0XFA83, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000,
  0X0000, 0X0000, 0X0000, 0XFA83, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000,
  0X0000, 0X0000, 0X0000, 0X0000, 0XFA83, 0X0000, 0X0000, 0X0000, 0X0000,
  0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0XFA83 };

const uint8_t PROGMEM gamma5[] = {
  0X00, 0X00, 0X00, 0X00, 0X01, 0X02, 0X03, 0X05, 0X07, 0X09, 0X0C, 0X10,
  0X14, 0X18, 0X1E, 0X24, 0X2B, 0X32, 0X3B, 0X44, 0X4E, 0X59, 0X65, 0X72,
  0X80, 0X8F, 0X9F, 0XB0, 0XC2, 0XD5, 0XE9, 0XFF };

const uint8_t PROGMEM gamma6[] = {
  0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X01, 0X01, 0X01, 0X02, 0X02,
  0X03, 0X04, 0X04, 0X05, 0X06, 0X07, 0X09, 0X0A, 0X0C, 0X0D, 0X0F, 0X11,
  0X13, 0X15, 0X17, 0X1A, 0X1D, 0X1F, 0X22, 0X26, 0X29, 0X2C, 0X30, 0X34,
  0X38, 0X3D, 0X41, 0X46, 0X4B, 0X50, 0X55, 0X5B, 0X61, 0X67, 0X6D, 0X74,
  0X7A, 0X81, 0X89, 0X90, 0X98, 0XA0, 0XA8, 0XB1, 0XBA, 0XC3, 0XCC, 0XD6,
  0XE0, 0XEA, 0XF4, 0XFF };

