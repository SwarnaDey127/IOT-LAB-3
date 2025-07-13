#include <SoftwareSerial.h>


// RX = D6 (GPIO12), TX = D5 (GPIO14)
SoftwareSerial uart(D6, D5);


unsigned long startTime, endTime;
int totalSent = 0, totalReceived = 0, errorCount = 0;


String generateMessage(int seq, int size) {
  String msg = String(seq) + ":D";
  while (msg.length() < size) msg += "X";
  return msg;
}


void performTest(long baud, int msgSize, int interval, int numMessages = 100) {
  Serial.println("-------------------------------------------------");
  Serial.println("Starting Test:");
  Serial.print("Baud: "); Serial.print(baud);
  Serial.print(", Size: "); Serial.print(msgSize);
  Serial.print(", Interval: "); Serial.print(interval); Serial.println("ms");


  // Send baud command to Slave first
  uart.println("BAUD:" + String(baud));
  delay(1000);  // Allow Slave to process baud change


  // Restart UART on new baud rate
  uart.end();
  delay(100);
  uart.begin(baud);
  delay(500);


  totalSent = 0;
  totalReceived = 0;
  errorCount = 0;
  startTime = millis();


  for (int i = 1; i <= numMessages; i++) {
    String msg = generateMessage(i, msgSize);
    uart.println(msg);
    totalSent++;


    unsigned long timeoutStart = millis();
    while (!uart.available() && millis() - timeoutStart < 100) {
      // wait up to 100 ms
    }


    if (uart.available()) {
      String response = uart.readStringUntil('\n');
      response.trim();
      if (response == msg) {
        totalReceived++;
      } else {
        errorCount++;
        Serial.print("Mismatch at #"); Serial.print(i);
        Serial.print(" | Sent: "); Serial.print(msg);
        Serial.print(" | Got: "); Serial.println(response);
      }
    } else {
      errorCount++;
      Serial.print("Timeout error at #"); Serial.println(i);
    }


    delay(interval); // wait before sending next message
  }


  endTime = millis();


  float testDurationSec = (endTime - startTime) / 1000.0;
  float throughput = (msgSize * totalReceived) / testDurationSec;
  float msgRate = totalReceived / testDurationSec;
  float errorRate = 100.0 * errorCount / totalSent;


  Serial.println("Test Results:");
  Serial.print("Messages Sent: "); Serial.println(totalSent);
  Serial.print("Messages Received: "); Serial.println(totalReceived);
  Serial.print("Errors: "); Serial.println(errorCount);
  Serial.print("Error Rate: "); Serial.print(errorRate); Serial.println(" %");
  Serial.print("Throughput: "); Serial.print(throughput); Serial.println(" bytes/sec");
  Serial.print("Message Rate: "); Serial.print(msgRate); Serial.println(" msg/sec");
}


void setup() {
  Serial.begin(115200); // For Serial Monitor
  uart.begin(9600);     // Initial baud for communication
  delay(1000);


  long baudRates[] = {9600, 38400, 115200};
  int messageSizes[] = {10, 50, 100};
  int intervals[] = {0, 10, 100};


  for (int b = 0; b < 3; b++) {
    for (int s = 0; s < 3; s++) {
      for (int d = 0; d < 3; d++) {
        performTest(baudRates[b], messageSizes[s], intervals[d]);
        delay(2000); // Small delay before next test
      }
    }
  }


  Serial.println("All tests completed.");
}


void loop() {
  // Nothing here — test runs once in setup
}
