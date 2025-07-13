#include <SoftwareSerial.h>


SoftwareSerial uart(D6, D5); // RX, TX
long currentBaud = 9600;


void changeBaudRate(long newBaud) {
  uart.end();
  delay(100);
  uart.begin(newBaud);
  currentBaud = newBaud;
  Serial.print("Changed baud to: ");
  Serial.println(newBaud);
}


void setup() {
  Serial.begin(115200);
  uart.begin(currentBaud);
  Serial.println("Slave ready at 9600");
}


void loop() {
  if (uart.available()) {
    String msg = uart.readStringUntil('\n');
    msg.trim();


    if (msg.startsWith("BAUD:")) {
      long newBaud = msg.substring(5).toInt();
      changeBaudRate(newBaud);
    } else {
      uart.println(msg);  // Echo back
      Serial.print("Received and echoed: ");
      Serial.println(msg);
    }
  }
}
