#ifndef _BLEMIDI_H_
#define _BLEMIDI_H_

#include <string>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

class BLEMidi {
public:
    BLEServer *server;
    BLEAdvertising *advertising;
    BLECharacteristic *characteristic;

    std::string deviceName;
    std::string midiServiceUUID;
    std::string midiCharacteristicUUID;

    bool isConnected = false;

    void begin(std::string deviceName,
        std::string midiServiceUUID,
        std::string midiCharacteristicUUID,
        BLEServerCallbacks *serverCallback,
        BLECharacteristicCallbacks *chatacteristicCallback);

    void sendNote(uint8_t status, uint8_t noteNo, uint8_t vel);
};

class securityCallbacks: public BLESecurityCallbacks {
    bool onConfirmPIN(uint32_t pin){
        return false;
    }
  
	uint32_t onPassKeyRequest(){
        //ESP_LOGI(LOG_TAG, "PassKeyRequest");
		return 123456;
	}

	void onPassKeyNotify(uint32_t pass_key){
        //ESP_LOGI(LOG_TAG, "On passkey Notify number:%d", pass_key);
	}

	bool onSecurityRequest(){
	    //ESP_LOGI(LOG_TAG, "On Security Request");
		return true;
	}

	void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl){
		//ESP_LOGI(LOG_TAG, "Starting BLE work!");
		if(cmpl.success){
			uint16_t length;
			esp_ble_gap_get_whitelist_size(&length);
			//ESP_LOGD(LOG_TAG, "size: %d", length);
		}
	}
};

extern BLEMidi Midi;

#endif