# SerialViaTelnet ESP32 / ESP8266

This Arduino library provides a buffered serial interface via telnet.

SerialViaTelnet is derived from the Stream class, so all methods known from `Serial` should work.

If there is no telnet client connected the output is stored in a buffer.

Once a client connects, the buffer is send before live data.

So the printf's during startup can be observed later on.

#### Changes in your code

```
#include <SerialViaTelnet.h>

setup() {
    SerialTelnet.begin(115200);
    
    SerialTelnet.println("my setup code is running");
}

loop() {
    SerialTelnet.loop();
}    
```

#### Print via telnet by class instance
```
SerialTelnet.printf("example %d\n", x);
```
#### Or print via telnet and keep existing code
```
#define Serial SerialTelnet

Serial.printf("example %d\n", x);
```
#### Send via hardware UART
```
SerialTelnet.HWSerial.printf("example %d\n", x);
```
#### Configuration
**CFG_CR**

   println() already appends \r\n to the line - nothing done.
   
   printf("\n") results in \n. CFG_CR substitude \n with \r\n
   
**CFG_FLUSH_RX**

   Any input data were flushed when loop() is called.
   
   available(), peek() or read() will return wrong results.
   
**CFG_SEND_INFO**

   Additional information is send on connect and whenever data is send to the ESP see emitInfo().
   If CFG_HW_SERIAL is disabled the information is send whenever a line is received.
   
**CFG_HW_SERIAL**

   The hardware UART can be used via HWSerial when Serial is overwritten by #define.
```
   SerialTelnet.HWSerial.println()
```
#### Installation
Download the library as [.zip file](https://github.com/ATho95/SerialViaTelnet/archive/main.zip)

Follow the Arduino instruction [how to add a .zip library](https://www.arduino.cc/en/guide/libraries#toc4)

#### License MIT
Copyright <2021> @ATho95

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
