#ifndef KEY_IN_WIFI_HPP
#define KEY_IN_WIFI_HPP

class String;
class Print;
class U8X8;

class KeyInWifi
{
public:
  enum class ButtonMode {
    NORMAL_LOW  = 0,
    NORMAL_HIGH = 1,
  };

  KeyInWifi(U8X8& u8x8, int btnPin, ButtonMode btnMode);

  void
  enableDebug(Print& debug);

  bool
  execute();

private:
  int
  promptMenu(char const* const* choices, int nChoices);

  String
  promptString();

  char
  promptChar(const char* chars);

private:
  Print* m_debug;
  U8X8& m_u8x8;
  int m_btnPin;
  ButtonMode m_btnMode;
};

#endif // KEY_IN_WIFI_HPP
