#include "Config.h"

#include <EEPROM.h>
#include <ESP8266WiFi.h>

namespace WiFiRemote {
  __Config Config;
}

void WiFiRemote::__Config::begin() {
  isValid = false;
  unsavedChanges = false;
  
  EEPROM.begin(512);

  if(loadInt(OFFSET_MAGIC, SIZE_MAGIC) == VALUE_MAGIC) {
    isValid = true;

    if(loadString(ssid, OFFSET_SSID, SIZE_SSID) == INVALID_STRING) {
      isValid = false;
    }
    else if(loadString(psk, OFFSET_PSK, SIZE_PSK) == INVALID_STRING) {
      isValid = false;
    }
    else if(loadString(serverName, OFFSET_SERVER_NAME, SIZE_SERVER_NAME) == INVALID_STRING) {
      isValid = false;
    }
    else if(loadString(serverPass, OFFSET_SERVER_PASS, SIZE_SERVER_PASS) == INVALID_STRING) {
      isValid = false;
    }
    else if(loadString(fieldDivision, OFFSET_FIELD_DIVISION, SIZE_FIELD_DIVISION) == INVALID_STRING) {
      isValid = false;
    }
    else {
      auto intOption = loadInt(OFFSET_MULTIPLE_FIELD, SIZE_MULTIPLE_FIELD);
      fieldOption = static_cast<MultipleFieldOption>(intOption);
    }
  }

  EEPROM.end();
}

void WiFiRemote::__Config::end() {
  if(unsavedChanges) {
    write();
  }
}

WiFiRemote::__Config::operator bool() const {
  return isValid;
}

String WiFiRemote::__Config::toString() const {
  String str;
  
  if(isValid) {
    str = String("SSID:\t") + ssid + "\n"
      + "PSK:\t" + psk + "\n"
      + "Server Name:\t" + serverName + "\n"
      + "Server Pass:\t" + serverPass + "\n"
      + "Field Division:\t" + fieldDivision + "\n"
      + "Multiple Field Option:\t";
    if(fieldOption == MultipleFieldOption::One) {
      str += "One";
    }
    else if(fieldOption == MultipleFieldOption::Two) {
      str += "Two";
    }
    else {
      str += "Both";
    }
  }
  else {
    str = "Invalid __Configuration";
  }

  return str;
}

String WiFiRemote::__Config::getSSID() const {
  return ssid;
}

String WiFiRemote::__Config::getPSK() const {
  return psk;
}

String WiFiRemote::__Config::getServerName() const {
  return serverName;
}

String WiFiRemote::__Config::getServerPass() const {
  return serverPass;
}

String WiFiRemote::__Config::getFieldDivision() const {
  return fieldDivision;
}

WiFiRemote::__Config::MultipleFieldOption WiFiRemote::__Config::getMultipleFieldOption() const {
  return fieldOption;
}

bool WiFiRemote::__Config::setSSID(const String& ssid) {
  if(ssid.length() < SIZE_SSID) {
    this->ssid = ssid;
    unsavedChanges = true;
    return true;
  }
  else {
    return false;
  }
}
bool WiFiRemote::__Config::setPSK(const String& psk) {
  if(psk.length() < SIZE_PSK) {
    this->psk = psk;
    unsavedChanges = true;
    return true;
  }
  else {
    return false;
  }
}
bool WiFiRemote::__Config::setServerName(const String& serverName) {
  if(serverName.length() < SIZE_SERVER_NAME) {
    this->serverName = serverName;
    unsavedChanges = true;
    return true;
  }
  else {
    return false;
  }
}
bool WiFiRemote::__Config::setServerPass(const String& serverPass) {
  if(serverPass.length() < SIZE_SERVER_PASS) {
    this->serverPass = serverPass;
    unsavedChanges = true;
    return true;
  }
  else {
    return false;
  }
}
bool WiFiRemote::__Config::setFieldDivision(const String& fieldDivision) {
  if(ssid.length() < SIZE_FIELD_DIVISION) {
    this->fieldDivision = fieldDivision;
    unsavedChanges = true;
    return true;
  }
  else {
    return false;
  }
}
bool WiFiRemote::__Config::setMultipleFieldOption(MultipleFieldOption fieldOption) {
  this->fieldOption = fieldOption;
  unsavedChanges = true;
  return true;
}

void WiFiRemote::__Config::write() {
  if(unsavedChanges) {
    EEPROM.begin(512);

    if(!isValid) {
      writeInt(OFFSET_MAGIC, VALUE_MAGIC, SIZE_MAGIC);
      isValid = true;
    }

    writeString(OFFSET_SSID, ssid);
    writeString(OFFSET_PSK, psk);
    writeString(OFFSET_SERVER_NAME, serverName);
    writeString(OFFSET_SERVER_PASS, serverPass);
    writeString(OFFSET_FIELD_DIVISION, fieldDivision);
    writeInt(OFFSET_MULTIPLE_FIELD, static_cast<int>(fieldOption), SIZE_MULTIPLE_FIELD);

    EEPROM.commit();
    EEPROM.end();

    unsavedChanges = false;
  }
}

int WiFiRemote::__Config::loadString(String& out, int offset, int totalStoreSize) {
  if(out.length() != 0) {
    out = "";
  }

  for(int i = 0; i <= totalStoreSize; ++i) {
    char c;
    EEPROM.get(offset + i, c);

    if(c == '\0') {
      break;
    }
    else if(i == totalStoreSize) {
      return INVALID_STRING;
    }
    else {
      out += c;
    }
  }

  return out.length();
}

int WiFiRemote::__Config::loadInt(int offset, int totalStoreSize) {
  int value = 0;
  
  if(totalStoreSize > sizeof(int)) {
    totalStoreSize = sizeof(int);
  }

  for(int i = 0; i < totalStoreSize; ++i) {
    int shamt = 8*i;

    uint8_t b;
    EEPROM.get(offset + i, b);
    value |= b << shamt;
  }

  return value;
}

void WiFiRemote::__Config::writeString(int offset, const String& str) {
  for(int i = 0; i < str.length(); ++i) {
    EEPROM.put(offset + i, str[i]);
  }
  EEPROM.put(offset + str.length(), (char)'\0');
}

void WiFiRemote::__Config::writeInt(int offset, int value, int size) {
  if(size > sizeof(int)) {
    size = sizeof(int);
  }

  for(int i = 0; i < size; ++i) {
    int shamt = 8*i;

    EEPROM.put(offset + i, (value >> shamt) & 0xFF);
  }
}

String WiFiRemote::__Config::getMAC() const {
  auto addr = WiFi.macAddress();
  String str;
  
  for(int i = 0; i < 5; ++i) {
    str += String(addr[i], 16) + ":";
  }
  str += String(addr[5], 16);

  return str;
}
