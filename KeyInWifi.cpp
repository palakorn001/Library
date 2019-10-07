#include "KeyInWifi.hpp"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <WiFi.h>
#include <WString.h>
#include <U8x8lib.h>

#define DELAY_CHOICE 250
#define DELAY_STEP 1500

KeyInWifi::KeyInWifi(U8X8& u8x8, int btnPin, ButtonMode btnMode)
  : m_debug(nullptr)
  , m_u8x8(u8x8)
  , m_btnPin(btnPin)
  , m_btnMode(btnMode)
{
}

void
KeyInWifi::enableDebug(Print& debug)
{
  m_debug = &debug;
}

#define DEBUGF(fmt, ...) do { if (m_debug != nullptr) { m_debug->printf_P(PSTR("%d [KeyInWifi] " fmt "\n"), millis(), ##__VA_ARGS__); } } while (false)

bool
KeyInWifi::execute()
{
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  switch (m_btnMode) {
    case ButtonMode::NORMAL_LOW:
      pinMode(m_btnPin, INPUT_PULLDOWN);
      break;
    case ButtonMode::NORMAL_HIGH:
      pinMode(m_btnPin, INPUT_PULLUP);
      break;
  }

  int nNetworks = WiFi.scanNetworks();
  nNetworks = std::min(nNetworks, static_cast<int>(m_u8x8.getRows()));
  std::vector<String> ssids;
  ssids.resize(nNetworks);
  std::vector<const char*> cssids;
  cssids.resize(nNetworks);
  for (int i = 0; i < nNetworks; ++i) {
    ssids[i] = WiFi.SSID(i);
    cssids[i] = ssids[i].c_str();
  }
  WiFi.scanDelete();
  int ssidIndex = this->promptMenu(cssids.data(), nNetworks);
  String ssid = ssids[ssidIndex];
  ssids.clear();
  cssids.clear();

  m_u8x8.clear();
  m_u8x8.drawString(0, 0, "SSID is");
  m_u8x8.drawString(0, 1, ssid.c_str());
  m_u8x8.drawString(0, 2, "Enter password");
  delay(DELAY_STEP);
  String password = this->promptString();

  m_u8x8.clear();
  m_u8x8.drawString(0, 0, "Connecting to");
  m_u8x8.drawString(0, 1, ssid.c_str());
  m_u8x8.drawString(0, 2, "using password");
  m_u8x8.drawString(0, 3, password.c_str());
  m_u8x8.drawString(0, 4, "status=");

  WiFi.begin(ssid.c_str(), password.c_str());
  while (!WiFi.isConnected()) {
    m_u8x8.drawGlyph(8, 4, '0' + WiFi.status());
  }
  m_u8x8.drawString(0, 4, "IP=");
  m_u8x8.drawString(4, 4, WiFi.localIP().toString().c_str());
  return true;
}

int
KeyInWifi::promptMenu(char const* const* choices, int nChoices)
{
  assert(nChoices > 0);
  assert(nChoices <= m_u8x8.getRows());

  m_u8x8.clear();
  for (int i = 0; i < nChoices; ++i) {
    m_u8x8.drawString(1, i, choices[i]);
  }

  while (true) {
    for (int i = 0; i < nChoices; ++i) {
      m_u8x8.drawGlyph(0, (i - 1 + nChoices) % nChoices, ' ');
      m_u8x8.drawGlyph(0, i, '>');
      for (int t = 0; t < DELAY_CHOICE; ++t) {
        if (digitalRead(m_btnPin) != static_cast<int>(m_btnMode)) {
          return i;
        }
        delay(1);
      }
    }
  }
}

String
KeyInWifi::promptString()
{
  String s;
  while (true) {
    char const* const choices[] = {
      s.c_str(),
      "<-",
      "0123456789 +._-",
      "ABCDEFGHIJKLM",
      "NOPQRSTUVWXYZ",
      "abcdefghijklm",
      "nopqrstuvwxyz",
      "~!@#$%^&*()",
    };
    int choice = this->promptMenu(choices, 8);
    switch (choice) {
      case 0:
        return s;
      case 1:
        s.remove(s.length() - 1);
        break;
      case 2:
        s += this->promptChar("0123456789 +._-");
        break;
      case 3:
        s += this->promptChar("ABCDEFGHIJKLM");
        break;
      case 4:
        s += this->promptChar("NOPQRSTUVWXYZ");
        break;
      case 5:
        s += this->promptChar("abcdefghijklm");
        break;
      case 6:
        s += this->promptChar("nopqrstuvwxyz");
        break;
      case 7:
        s += this->promptChar("!\"#$%&'()*,/:;<=>?@[\\]^`{|}~");
    }
  }
}

char
KeyInWifi::promptChar(const char* chars)
{
  int nChars = std::strlen(chars);
  int nRows = m_u8x8.getRows();
  int nCols = m_u8x8.getCols();
  assert(nChars <= nRows * nCols / 2);

  m_u8x8.clear();
  for (int i = 0; i < nChars; ++i) {
    std::div_t xy = std::div(i, nRows);
    m_u8x8.drawGlyph(xy.quot * 2 + 1, xy.rem, chars[i]);
  }

  while (true) {
    for (int i = 0; i < nChars; ++i) {
      std::div_t xy = std::div((i - 1 + nChars) % nChars, nRows);
      m_u8x8.drawGlyph(xy.quot * 2, xy.rem, ' ');
      xy = std::div(i, nRows);
      m_u8x8.drawGlyph(xy.quot * 2, xy.rem, '>');
      for (int t = 0; t < DELAY_CHOICE; ++t) {
        if (digitalRead(m_btnPin) != static_cast<int>(m_btnMode)) {
          return chars[i];
        }
        delay(1);
      }
    }
  }
}

