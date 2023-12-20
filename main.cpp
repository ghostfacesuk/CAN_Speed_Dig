#include "mbed.h"
#include "CAN.h"
// #include <USBSerial.h>

// USBSerial pc;  // USB serial port

DigitalOut led1(LED1);  // MBED LED1
DigitalOut led4(LED4);  // MBED LED4
DigitalOut pin21(p21);  // Output for external LED and buzzer
CAN can1(p9, p10);      // CAN pins, CAN port 1

void convertTime(uint32_t rawTime, int &hours, int &minutes, float &seconds) {
    float timeInSeconds = static_cast<float>(rawTime) * 0.01;
    hours = static_cast<int>(timeInSeconds / 3600);
    timeInSeconds -= hours * 3600;
    minutes = static_cast<int>(timeInSeconds / 60);
    timeInSeconds -= minutes * 60;
    seconds = timeInSeconds;
}

int main() {
    printf("CAN Example\n");

    can1.frequency(500000);
    CANMessage msg;
    bool ledOn = false;
    uint16_t prevCanValue = 0;
    uint32_t canTime = 0;

    while (1) {
        if (can1.read(msg)) {
            if (msg.id == 0x301 && msg.type == CANData && msg.format == CANStandard && msg.len == 8) {
                // Extract the 24-bit unsigned integer from bytes 1, 2, and 3 in big-endian format
                canTime = ((uint32_t)msg.data[1] << 16) | ((uint32_t)msg.data[2] << 8) | msg.data[3];
            }
            if (msg.id == 0x302 && msg.type == CANData && msg.format == CANStandard && msg.len == 8) {
                // Extract the 16-bit unsigned integer from bytes 4 and 5 in big-endian format
                uint16_t canValue = ((((uint16_t)msg.data[4]) << 8) | msg.data[5]) * 0.01852;

                // Check for transitions from below 60 to above 60 or vice versa
                if ((canValue < 60 && prevCanValue >= 60) || (canValue >= 60 && prevCanValue < 60)) {
                    // Convert raw time to HH:MM:SS format
                    int hours, minutes;
                    float seconds;
                    convertTime(canTime, hours, minutes, seconds);

                    // Print the time, CAN value, and the transition
                    printf("Time: %02d:%02d:%06.3f, CAN Value Transition: %u\n", hours, minutes, seconds, canValue);
                }

                // Check if the received value is greater than or equal to 60
                if (canValue >= 60) {
                    if (!ledOn) {
                        // Turn ON the LED only if it's currently OFF
                        led1 = 1;
                        pin21 = 1;
                        ledOn = true;
                        led4 = 0;
                    }
                } else {
                    if (ledOn) {
                        // Turn OFF the LED only if it's currently ON
                        led1 = 0;
                        pin21 = 0;
                        ledOn = false;
                        led4 = 0;
                    }
                }

                prevCanValue = canValue; // Update previous CAN value
            } else {
                // Print an error message when there is an error in the received CAN message
                led4 = 1;
            }
        }
    }
}