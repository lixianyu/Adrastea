/**
 * @example TCPClientSingle.ino
 * @brief The TCPClientSingle demo of library WeeESP8266. 
 * @author Wu Pengfei<pengfei.wu@itead.cc> 
 * @date 2015.02
 * 
 * @par Copyright:
 * Copyright (c) 2015 ITEAD Intelligent Systems Co., Ltd. \n\n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version. \n\n
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "ESP8266.h"
//#define Serial_DEBUG
#ifdef Serial_DEBUG
#define JOY_LOG(x) Serial.print(x)
#define JOY_LOGLN(x) Serial.println(x)
#else
#define JOY_LOG(x) 
#define JOY_LOGLN(x) 
#endif

#define SSID        "esp32-ap"
#define PASSWORD    "MakerModule"
#define HOST_NAME   "192.168.10.1"
#define HOST_PORT   (4567)

ESP8266 wifi(Serial1);

void setup(void)
{
    Serial.begin(115200);
    //Serial.print("setup begin\r\n");
    JOY_LOG("setup begin\r\n");
    
    JOY_LOG("FW Version:");
    JOY_LOGLN(wifi.getVersion().c_str());
    delay(1000);
    if (wifi.setOprToStation()) {
		JOY_LOG("to station ok\r\n");
	} else {
		JOY_LOG("to station err\r\n");
	}
 
    if (wifi.joinAP(SSID, PASSWORD)) {
        JOY_LOG("Join AP success\r\n");
        JOY_LOG("IP:");
        JOY_LOGLN( wifi.getLocalIP().c_str());       
    } else {
        JOY_LOG("Join AP failure\r\n");
    }
    
    if (wifi.disableMUX()) {
        JOY_LOG("single ok\r\n");
    } else {
        JOY_LOG("single err\r\n");
    }
#if 1
	if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
        JOY_LOG("create tcp ok\r\n");
    } else {
        JOY_LOG("create tcp err\r\n");
    }
#endif
	Serial1.println("AT+CIPMODE=1"); // Transparent transmission
	delay(500);
	Serial1.println("AT+CIPSEND");
	delay(500);
    JOY_LOG("setup end\r\n");
}

#if 0
void loop(void)
{
    uint8_t buffer[128] = {0};
    #if 1
    if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
        Serial.print("create tcp ok\r\n");
    } else {
        Serial.print("create tcp err\r\n");
        delay(1000);
        return;
    }
    #endif
	snprintf((char*)buffer, sizeof(buffer), "Hello, this is client!...%ld", millis());

    //char *hello = "Hello, this is client!";
    wifi.send((const uint8_t*)buffer, strlen((char*)buffer));
    #if 1
    uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
    if (len > 0) {
        Serial.print("Received:[");
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
        }
        Serial.print("]\r\n");
    }
    #endif
    #if 1
    if (wifi.releaseTCP()) {
        Serial.print("release tcp ok\r\n");
    } else {
        Serial.print("release tcp err\r\n");
    }
	#endif
    delay(1000);
}
#else
void loop(void)
{
    uint8_t buffer[128] = {0};
    #if 0
    if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
        Serial.print("create tcp ok\r\n");
    } else {
        Serial.print("create tcp err\r\n");
        delay(1000);
        return;
    }
    #endif
	snprintf((char*)buffer, sizeof(buffer), "Hello, this is client!...%ld", millis());
	
    //char *hello = "Hello, this is client!";
    //wifi.send((const uint8_t*)buffer, strlen((char*)buffer));
    uint32_t len = strlen((char*)buffer);
	len = len > sizeof(buffer) ? sizeof(buffer) - 1 : len;
    for (uint32_t i = 0; i < len; i++) {
		Serial1.write(buffer[i]);
	}
    #if 0
    uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
    if (len > 0) {
        Serial.print("Received:[");
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
        }
        Serial.print("]\r\n");
    }
    #endif
    #if 0
    if (wifi.releaseTCP()) {
        Serial.print("release tcp ok\r\n");
    } else {
        Serial.print("release tcp err\r\n");
    }
	#endif
    delay(1000);
}
#endif
