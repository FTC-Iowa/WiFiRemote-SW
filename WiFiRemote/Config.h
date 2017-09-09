#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

namespace WiFiRemote {
  class Config {
  public:
    enum class MultipleFieldOption {
      One = 0,
      Two = 1,
      Both = 2
    };
  
    Config();
    ~Config();
  
    /*! \brief Returns true if EEPROM has valid configuration
     * 
     */
    operator bool() const;

    String toString() const;
  
    String getSSID() const;
    String getPSK() const;
    String getServerName() const;
    String getServerPass() const;
    String getFieldDivision() const;
    MultipleFieldOption getMultipleFieldOption() const;
  
    bool setSSID(const String& ssid);
    bool setPSK(const String& psk);
    bool setServerName(const String& name);
    bool setServerPass(const String& pass);
    bool setFieldDivision(const String& division);
    bool setMultipleFieldOption(MultipleFieldOption option);
    void write();
  
  private:
    const int INVALID_STRING = -1;
    
    const int OFFSET_MAGIC = 0;
    const int OFFSET_SSID = 4;
    const int OFFSET_PSK = 37;
    const int OFFSET_SERVER_NAME = 101;
    const int OFFSET_SERVER_PASS = 165;
    const int OFFSET_FIELD_DIVISION = 229;
    const int OFFSET_MULTIPLE_FIELD = 293;

    const int SIZE_MAGIC = 4;
    const int SIZE_SSID = 33;
    const int SIZE_PSK = 64;
    const int SIZE_SERVER_NAME = 64;
    const int SIZE_SERVER_PASS = 64;
    const int SIZE_FIELD_DIVISION = 64;
    const int SIZE_MULTIPLE_FIELD = 1;

    const int VALUE_MAGIC = 0xb01dface;

    int loadString(String& out, int offset, int totalStoreSize);
    int loadInt(int offset, int totalStoreSize);

    void writeString(int offset, const String& str);
    void writeInt(int offset, int value, int size);
    
    String ssid, psk;
    String serverName, serverPass;
    String fieldDivision;
    MultipleFieldOption fieldOption;

    bool isValid;
    bool unsavedChanges;
  };
}


#endif
