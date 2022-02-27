#include <Ethernet.h>
#include <EthernetUdp.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(169,254,214,200);

unsigned int localPort = 54000;      // local port to listen on

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,
char ReplyBuffer[] = "HELLO PYTHON";        // a string to send back

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP UDP;

void setup() {
  // You can use Ethernet.init(pin) to configure the CS pin
  Ethernet.init(10);  // Most Arduino shields
  
  // start the Ethernet
  Ethernet.begin(mac, ip);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start UDP
  UDP.begin(localPort);
}

void loop() {
  // if there's data available, read a packet
  int packetSize = UDP.parsePacket();
  if (packetSize) {
    /*
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    */
    Serial.print("CLIENT IP: ");
    IPAddress remote = UDP.remoteIP();
    for (int i=0; i < 4; i++) {
      Serial.print(remote[i], DEC);
      if (i < 3) {
        Serial.print(".");
      }
    }
    Serial.print("\nCLIENT PORT: ");
    Serial.println(UDP.remotePort());

    // read the packet into packetBufffer
    UDP.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    Serial.print("MESSAGE FROM CLIENT: ");
    Serial.println(packetBuffer);

    // send a reply to the IP address and port that sent us the packet we received
    UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
    UDP.write(ReplyBuffer);
    UDP.endPacket();
  }
  delay(10);
}
