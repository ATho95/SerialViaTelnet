/*
 * SerialViaTelnet.h
 *
 * Version 1.0
 *
 * use:
 * SerialTelnet.printf("example %d\n", x);
 * ---------------------------------------
 *
 * or over#define existing code:
 * #define Serial SerialTelnet
 * Serial.printf("example %d\n", x);
 * ---------------------------------
 *
 * or send to UART:
 * SerialTelnet.HWSerial.printf("example %d\n", x);
 * ------------------------------------------------
 *
 * If there is no telnet client connected the output is stored in a buffer.
 * Once a client connects, the buffer is send before live data. So the printf's
 * during startup can be observed later on.
 *
 * Configuration:
 * CFG_CR
 *    println() appends /r/n to the line
 *    printf("\n") sends /n. If enabled \r\n is send
 * CFG_FLUSH_RX
 *    Any input data were flushed when loop() is called
 *    available(), peek() or read() will return wrong results
 * CFG_SEND_INFO
 *    Additional information is send on connect and whenever data is send to the
 * ESP
 *    -> emitInfo()
 * CFG_HW_SERIAL
 *    The hardware UART can be used via HWSerial
 *    -> SerialTelnet.HWSerial.println()
 *
 *  Created on: 10.01.2019
 *      Author: Andreas
 */

#ifndef TELNETVIASERIAL_H_
#define TELNETVIASERIAL_H_

#define CFG_CR 1        // 1 = append CR to each LF
#define CFG_FLUSH_RX 0  // 1 = the input buffer is automatically emptied
#define CFG_SEND_INFO 1 // 1 = info on telnet data receive
#define CFG_HW_SERIAL 1 // 1 = forward begin() to Serial.begin()
#define CFG_TELNET_PORT 23

#define BOOT_MSG_SIZE 1000 // if not connected write to bootMsg[]
#define MAX_LINE_SIZE 80   // size of temporary buffer

#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <WiFiServer.h>

class SerialViaTelnet : public Stream {
private:
    bool clientConnected;
    bool writeToBuffer, bufferFull;
    uint8_t bootMsg[BOOT_MSG_SIZE];
    WiFiServer TelnetServer = WiFiServer(CFG_TELNET_PORT);
    WiFiClient TelnetClient;
    bool sendPrefix = false;
    bool wifiConnected = false;
    bool isConnected();
    void cleanup();
    size_t emitData(const uint8_t *buffer, size_t size);
    void checkInput();
#if CFG_SEND_INFO
    void emitInfo();
#endif

public:
#if CFG_HW_SERIAL
    HardwareSerial HWSerial = Serial;
#endif
    SerialViaTelnet();
    ~SerialViaTelnet();
    void begin(uint32_t baud);
    void loop();
    int available() override;
    int read() override;
    int peek() override;
    void flush() override;
    size_t write(uint8_t c) override;
    size_t write(const uint8_t *buffer, size_t size) override;
};

extern SerialViaTelnet SerialTelnet;

#endif /* TELNETVIASERIAL_H_ */
