#include <SPI.h>
#define CAN_2515
#include "mcp2515_can.h"

const int SPI_CS_PIN = 10;
const int CAN_INT_PIN = 2;

String lvStr;
char lvCh;

String currentStr;
mcp2515_can CAN(SPI_CS_PIN);

void send(unsigned char highByte, unsigned char lowByte) {
    unsigned char stmp[8] = {0x22, 0x45, 0x20, 0x01, lowByte, highByte, 0x00, 0x00};
    CAN.sendMsgBuf(0x601, 0, 8, stmp);
    delay(100);  // send data every 100ms
    Serial.println("CAN BUS sendMsgBuf ok!");
}

void check() {
    unsigned char len = 0;
    unsigned char buf[8];

    if (CAN_MSGAVAIL == CAN.checkReceive()) {  // check if data is coming
        CAN.readMsgBuf(&len, buf);  // read data

        unsigned long canId = CAN.getCanId();
        Serial.println("-----------------------------");
        Serial.print("Get data from ID: 0x");
        Serial.println(canId, HEX);

        for (int i = 0; i < len; i++) {
            Serial.print(buf[i], HEX);
            Serial.print("\t");
        }
        Serial.println();
    }
}

void setup() {
    Serial.begin(115200);
    while (!Serial) {};

    while (CAN_OK != CAN.begin(CAN_500KBPS)) {  // initialize CAN bus with baud rate = 500k
        Serial.println("CAN init fail, retry...");
        delay(100);
    }
    Serial.println("CAN init ok!");
}

void loop() {
    if (Serial.available()) {
        lvCh = Serial.read();
        lvStr += lvCh;

        
        if (lvCh == '\n') {
          
            for (int i = 0; i < 4; i++) {
                currentStr += lvStr[i];
            }

            float currentValue = currentStr.toFloat();
            float result = currentValue * 1.092267;
            unsigned int intResult = static_cast<unsigned int>(round(result));

            unsigned char highByte = (intResult >> 8) & 0xFF;  // extract high byte
            unsigned char lowByte = intResult & 0xFF;           // extract low byte

          
                send(highByte, lowByte);
                Serial.println(lvStr);
                Serial.println(currentStr);
                Serial.println(highByte, HEX);
                Serial.println(lowByte, HEX);
            
            lvStr = "";
            currentStr = "";
        }
    }

    check();
}