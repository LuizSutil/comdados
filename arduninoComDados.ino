

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <MFRC522.h>


constexpr uint8_t RST_PIN =  D3;          // Configurable, see typical pin layout above 18
constexpr uint8_t SS_PIN =  D8;         // Configurable, see typical pin layout above  16

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
// Update these with values suitable for your network.

const char* ssid = "Luizeira";
const char* password = "luizehlegal";
const char* mqtt_server = "broker.hivemq.com";

#define ID_MQTT  "ABP_IOT_LUIZ"    

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup() {
   Serial.println("Setup");

  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);

 
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  
  delay(1000);
  Serial.println("Setup");
  
  while (!Serial);   
  SPI.begin();      
  mfrc522.PCD_Init();  
  mfrc522.PCD_DumpVersionToSerial(); 
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
  Serial.println("Setup done");

}


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if (client.connect(ID_MQTT)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("satc/iot/luiz", "hello world");
      // ... and resubscribe
      client.subscribe("satc/iot/luiz");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



void loop() {

       // Look for new cards
      if ( ! mfrc522.PICC_IsNewCardPresent()) {
        delay(50);
        return;
      }
    
      // Select one of the cards
      if ( ! mfrc522.PICC_ReadCardSerial()) {
        delay(50);
        return;
        }

   String content= "";

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  unsigned long now = millis();
  if (now - lastMsg > 6000) {
    
         for (byte i = 0; i < mfrc522.uid.size; i++) 
          {
    
             content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
             content.concat(String(mfrc522.uid.uidByte[i], HEX));
          }
          content.toUpperCase();
              byte letter;
         
       
        if (content.substring(1) == "67 A8 96 60") //change here the UID of the card/cards that you want to give access
        {

         
          client.publish("satc/comdados", "0");
                    
          Serial.println(content.substring(1));

          delay(1000);
        } else
         {
          Serial.println(content.substring(1));

          client.publish("satc/comdados", "1");
          delay(1000);

        }

  }
}
