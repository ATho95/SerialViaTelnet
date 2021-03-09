/*
 * SerialViaTelnet.cpp
 *
 *  Created on: 10.01.2019
 *      Author: Andreas
 */

#include <SerialViaTelnet.h>
#ifdef ESP32
#include <rom/rtc.h>
#endif

SerialViaTelnet::SerialViaTelnet() { cleanup(); }

SerialViaTelnet::~SerialViaTelnet() {
    if (isConnected()) {
        TelnetClient.stop();
    }
}

void SerialViaTelnet::begin(uint32_t baud) {
#if CFG_HW_SERIAL
    HWSerial.begin(baud);
    HWSerial.println();
#endif
}

bool SerialViaTelnet::isConnected() {
    if (!wifiConnected) {
        if (WiFi.isConnected()) {
            wifiConnected = true;
            TelnetServer.begin((uint16_t)CFG_TELNET_PORT);
            TelnetServer.setNoDelay(true);
        } else {
            return false;
        }
    }
    return TelnetClient.connected();
}

void SerialViaTelnet::cleanup() {
    memset(bootMsg, 0, BOOT_MSG_SIZE);
    clientConnected = false;
    writeToBuffer = true;
    bufferFull = false;
}

size_t SerialViaTelnet::write(uint8_t c) {
    uint8_t buffer[4];
    buffer[0] = c;
    return write(buffer, 1);
}

size_t SerialViaTelnet::write(const uint8_t *buffer, size_t size) {
    if (writeToBuffer) {
        if (isConnected()) {
            // Telnet connect status has changed
            writeToBuffer = false;
            sendPrefix = true;
            println("\nESP Telnet");
            emitInfo();
            if (bootMsg[0] != '\0') {
                println(">>> buffer start >>>");
                emitData((const uint8_t *)bootMsg,
                         (size_t)strlen((const char *)bootMsg));
                printf("<<< buffer %s <<<\n",
                       bufferFull ? "/ overflow" : "end");
            }
            sendPrefix = false;
        }
        if (writeToBuffer && (!bufferFull)) { // still true ?
            int end = strlen((const char *)bootMsg);
            if ((end + size) < (BOOT_MSG_SIZE - 4)) {
                memcpy((char *)bootMsg + end, (const char *)buffer, size);
                bootMsg[end + size] = '\0';
            } else {
                bufferFull = true;
            }
        }
    }
    return emitData(buffer, size);
}

size_t SerialViaTelnet::emitData(const uint8_t *buffer, size_t size) {
    size_t len = 0;
    if (isConnected()) {
#if CFG_CR
        uint8_t tmp[MAX_LINE_SIZE];
        size_t index = 0;
        bool cr, lf;

        while (size > 0) {
            cr = lf = false;
            while ((index < (MAX_LINE_SIZE - 4)) && (index < size)) {
                tmp[index++] = *buffer;
                if (*buffer == '\r') {
                    cr = true;
                }
                if (*buffer++ == '\n') {
                    lf = true;
                    break;
                }
            }
            size -= index;
            if (lf && (!cr)) {
                tmp[index++] = '\r';
            }
            len += TelnetClient.write(tmp, index);
            // HWSerial.write(tmp, index);
            if (sendPrefix && lf) {
                TelnetClient.write("# ", 2);
                // HWSerial.write("# ", 2);
            }
            index = 0;
        }
#else
        len = TelnetClient.write((const char *)buffer, size);
#endif
    }
    return len;
}

void SerialViaTelnet::loop() {
    // Cleanup disconnected session
    if (clientConnected && !isConnected()) {
        TelnetClient.stop();
        cleanup();
        // HWSerialprintln("telnet disconnect");
    }

    // Check new client connections
    if (TelnetServer.hasClient()) {
        if (isConnected()) {
            TelnetServer.available().println("\r\nSorry, no free session");
            TelnetServer.available().stop();
            println("\nAn other user wants to connect\n");
        } else {
            TelnetClient = TelnetServer.available();
            clientConnected = true;
            // HWSerialprintln("telnet connect");
            println(); // trigger sending buffered data
            while (TelnetClient.available()) {
                // empty RX data
                TelnetClient.read();
            }
            return;
        }
    }
#if CFG_FLUSH_RX
    if (TelnetClient.available()) {
        // clear RX data
        while (read() > 0) {
        }
#if CFG_SEND_INFO
        emitInfo();
#endif
    }
#endif
}

int SerialViaTelnet::available() { return TelnetClient.available(); }
int SerialViaTelnet::read() { return TelnetClient.read(); }
int SerialViaTelnet::peek() { return TelnetClient.peek(); }
void SerialViaTelnet::flush() { return TelnetClient.flush(); }

#if CFG_SEND_INFO
void SerialViaTelnet::emitInfo() {
    uint32_t ideSize = ESP.getFlashChipSize();
    FlashMode_t ideMode = ESP.getFlashChipMode();
    println("----------");
    printf("IP Addr:         %s\n", WiFi.localIP().toString().c_str());
    printf("MAC Addr:        %s\n", WiFi.macAddress().c_str());
#ifdef ESP32
    printf("Hostname:        %s\n", WiFi.getHostname());
    printf("Reset reason 0:  %u\n", rtc_get_reset_reason(0));
    printf("Reset reason 1:  %u\n", rtc_get_reset_reason(1));
    printf("Flash ide  size: %u\n", ideSize);
#else
    uint32_t realSize = ESP.getFlashChipRealSize();
    printf("Hostname:        %s\n", WiFi.hostname().c_str());
    printf("Reset reason:    %s\n", ESP.getResetReason().c_str());
    printf("Reset info:      %s\n", ESP.getResetInfo().c_str());
    printf("Flash real id:   0x%08X\n", ESP.getFlashChipId());
    printf("Flash real size: %u\n", realSize);
    printf("Flash size conf: %u\n", ideSize);
    printf("Configuration:   %s\n", ideSize != realSize ? "wrong!" : "OK");
#endif
    printf("Flash free:      %u\n", ESP.getFreeSketchSpace());
    printf("Flash ide speed: %u\n", ESP.getFlashChipSpeed());
    printf("Flash ide mode:  %s\n", ideMode == FM_QIO    ? "QIO"
                                    : ideMode == FM_QOUT ? "QOUT"
                                    : ideMode == FM_DIO  ? "DIO"
                                    : ideMode == FM_DOUT ? "DOUT"
                                                         : "UNKNOWN");
    printf("Free Heap:       %u\n", ESP.getFreeHeap());
    println("----------");
}
#endif

SerialViaTelnet SerialTelnet;
