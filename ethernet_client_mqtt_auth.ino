/*
 Basic MQTT example

 This sketch demonstrates the basic capabilities of the library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 
*/

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
//IPAddress ip(192, 168, 0, 100);
//IPAddress server(192, 168, 0, 182); //servidor al cual conectarse
const char* mqtt_server = "broker.shiftr.io";  //broker pc (ip de mi 
                                          //computador que tiene instalado mosquitto broker mqtt 

///////////////////////AUTENTICACION BROKER MQTT///////////////////////////////////////////
const char clientID[]="NODEMCU-MARKO";  //identificador unico de para cada dispositivo iot
const char username[]="marko1234";       //usuario configurado en broker
const char passwords[]="marko1234";      //contraseña usuario broker
const char willTopic[]= "status";
int willQoS=2 ;                             //0-1-2
int  willRetain=1 ;                         //0-1  //si se activa o no la retencion de data
const char willMessage[]="conected";    //mensaje cuando device este desconectado de broker
////////////////////////////////////////////////////////////////////////////////////////////////

EthernetClient ethClient;
PubSubClient client(ethClient);
long lastMsg = 0;
char msg[50];
int value = 0;


void callback(char* topic, byte* payload, unsigned int length) {
Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(13, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(13, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    //if (client.connect("arduinoClient")) {
     if(client.connect(clientID,username,passwords,willTopic,willQoS,willRetain,willMessage)){    
 
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic","hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(115200);


 
  Serial.println("");
  Serial.println("connected DHCP");

  Ethernet.begin(mac); //DHCP
  //Ethernet.begin(mac,ip); //STATIC IP

  Serial.println("IP address: ");
  Serial.println(Ethernet.localIP());
  // Allow the hardware to sort itself out

   Serial.print("Connecting to broker: ");
   Serial.print(mqtt_server);
   client.setServer(mqtt_server, 1883);
   client.setCallback(callback);

 Serial.println(" ");
   
  delay(1500);
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 50, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
  }
}

