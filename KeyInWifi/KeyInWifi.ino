#include <U8x8lib.h>
#include "KeyInWifi.hpp"

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(15, 4, 16);

void
setup()
{
  Serial.begin(115200);
  Serial.println();

  u8x8.begin();
  u8x8.setFont(u8x8_font_victoriamedium8_r);
  KeyInWifi kiw(u8x8, 0, KeyInWifi::ButtonMode::NORMAL_HIGH);
  kiw.enableDebug(Serial);
  kiw.execute();
}

void
loop()
{
}
