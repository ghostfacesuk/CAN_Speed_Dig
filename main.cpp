#include "mbed.h"
#include "CAN.h"

DigitalOut led1(LED1);  // MBED LED
DigitalOut pin21(p21);  // Output for external LED and buzzer
CAN can1(p9, p10);      // CAN pins, CAN port 1

int main() {
    printf("CAN Example\n");

    can1.frequency(500000);
    CANMessage msg;
    bool ledOn = false;

    while (1) {
        if (can1.read(msg) && msg.id == 0x302) {
            // Check if the message was received without errors
            if (msg.type == CANData && msg.format == CANStandard) {
                // Extract the 16-bit unsigned integer from bytes 4 and 5 in big-endian format
                uint16_t canValue = ((((uint16_t)msg.data[4]) << 8) | msg.data[5]) * 0.01852;

             //   printf("Received CAN message: ID = 0x%X, Value = %u\n", msg.id, canValue);

                // Check if the received value is greater than or equal to 60
                if (canValue >= 60) {
                    if (!ledOn) {
                        // Turn ON the LED only if it's currently OFF
                        led1 = 1;
                        pin21 = 1;
                        ledOn = true;
                        printf("Turning ON LED and Pin 21\n");
                    }
                } else {
                    if (ledOn) {
                        // Turn OFF the LED only if it's currently ON
                        led1 = 0;
                        pin21 = 0;
                        ledOn = false;
                        printf("Turning OFF LED and Pin 21\n");
                    }
                }
            } else {
             //   printf("Error in received CAN message\n");
            }
        }
    }
}
