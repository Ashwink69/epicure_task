#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Ashwin";
const char* password = "ashwin253469";
const char* mqtt_server = "172.20.10.3";

WiFiClient espClient;
PubSubClient client(espClient);

HardwareSerial STM(2);  // UART2 = GPIO16 (RX2), GPIO17 (TX2)
// If you are using GPIO1/3, then keep Serial2 instead.

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.println("\n=== MQTT MESSAGE RECEIVED ===");

  String payload = "";
  for (int i = 0; i < length; i++) {
    payload += (char)message[i];
  }

  Serial.print("MQTT Topic: ");
  Serial.println(topic);

  Serial.print("MQTT Payload: ");
  Serial.println(payload);

  // ------------------------------
  // Send to STM32 over UART
  // ------------------------------
  Serial.println("Sending to STM32 over UART...");
  STM.print(payload + "\n");
  delay(50);
  Serial.println("Message SENT to STM32 ✓");

  // ------------------------------
  // Read ANY reply from STM32 (if exists)
  // ------------------------------
  if (STM.available()) {
    Serial.print("STM32 replied: ");
    while (STM.available()) {
      char c = STM.read();
      Serial.print(c);
    }
    Serial.println();
  } else {
    Serial.println("No reply from STM32 (but UART TX successful).");
  }

  Serial.println("==============================");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected!");
      client.subscribe("epicure/commands");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 2 seconds...");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);     // USB serial monitor
  STM.begin(115200, SERIAL_8N1, 16, 17);  // UART2 for STM32

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  Serial.println("ESP32 Ready.");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
