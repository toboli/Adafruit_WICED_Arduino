/*
  MQTT Example:
  1.  Connect to pre-specified AP
  2.  Set the LastWill message
  3.  Connect to MQTT Broker
  4.  Subscribe to a specified topic
  5a. Publish "Alive" message every 15 seconds
  5b. Read messages (if exist) from subscribed topic

  author: huynguyen 
 */

#include "adafruit_wifi.h"

uint16_t wifi_error, mqtt_error, subs_error;

/**************************************************************************/
/*!
    @brief  Connect to pre-specified AP
    
    @return Error code  
*/
/**************************************************************************/
uint16_t connectAP()
{
  // Connect to an AP
  char* ssid = "huy-laptop";
  char* pass = "12345678";
  uint16_t error = wiced.connectAP(ssid, pass);
  if (error == ERROR_NONE)
  {
    Serial.print("Connected to AP with SSID = ");
    Serial.print(ssid);
    Serial.print(" and PASSWORD = ");
    Serial.println(pass);
  }
  else
  {
    Serial.println("Connect AP Error!");
  }
  Serial.println("");
  return error;
}

/**************************************************************************/
/*!
    @brief  Connect to pre-specified Broker

    @return Error code
*/
/**************************************************************************/
uint16_t connectBroker()
{
  char* host = "test.mosquitto.org";
  // Default clienID = "Adafruit"
  // Default port = 1883
  // No user name and password
  uint16_t error = wiced.mqttConnect(host, 0, NULL, NULL, NULL);
  if (error == ERROR_NONE)
  {
    Serial.print("Connected to Broker = ");
    Serial.println(host);
  }
  else
  {
    Serial.println("Connect Broker Error!");
  }
  Serial.println("");
  return error;
}

/**************************************************************************/
/*!
    @brief  Subscribe to a specified topic

    @return Error code
*/
/**************************************************************************/
uint16_t subscribeTopic()
{
  char* topic = "topic/adafruit/sensor";
  uint16_t error = wiced.mqttSubscribe(topic, 0);
  if (error == ERROR_NONE)
  {
    Serial.print("Subscribed to the topic = <");
    Serial.print(topic); Serial.println(">");
  }
  else
  {
    Serial.println("Subscribe Error!");
  }
  Serial.println("");
  return error;
}

/**************************************************************************/
/*!
    @brief  The setup function runs once when reset the board    
*/
/**************************************************************************/
void setup()
{
  pinMode(BOARD_LED_PIN, OUTPUT);
  
  // wait for Serial
  while (!Serial) delay(1);
  
  wifi_error = connectAP();
  mqtt_error = connectBroker();
  subs_error = subscribeTopic();

  // Set LastWill message
  if (wiced.mqttLastWill("topic/adafruit","Offline",0,0) == ERROR_NONE)
  {
    Serial.println("Specified LastWill Message!");
  }
  
  // initialize serial port for input and output
//  Serial.begin(11500);
}

/**************************************************************************/
/*!
    @brief  The loop function runs over and over again forever
*/
/**************************************************************************/
void loop() {
  if (wifi_error == ERROR_NONE)
  {
    if (mqtt_error == ERROR_NONE)
    {
      // Publish a Message to a Topic
      char* topic = "topic/adafruit";
      char* value = "Alive";
      // qos = 1, retain = 0
      if (wiced.mqttPublish(topic, value, 0, 0) == ERROR_NONE)
      {
        Serial.print("Published Message! ");
        Serial.print("Value = ");
        Serial.println(value);
      }

      if (subs_error == ERROR_NONE)
      {
        uint16_t irq_error;

        uint16_t n_item;
        irq_error = wiced.irqCount(&n_item);
        Serial.print("Number of items: ");
        Serial.println(n_item);

        // Read message from subscribed topic
        uint8_t response[64];
        uint16_t response_len;
        irq_error = wiced.irqRead(&response_len, response);
        if (irq_error == ERROR_NONE)
        {
          Serial.println("Message read!");
          for (int i = 10; i < response_len; i++)
          {
            Serial.write(response[i]);
          }
          Serial.println("");
        }
        else
        {
          Serial.println(irq_error, HEX);
        }
      }
      else
      {
        wiced.mqttUnsubscribe();
        // Try to subscribe to the topic again
        subs_error = subscribeTopic();
      }
    }
    else
    {
      // Try to reconnect to MQTT Server    
      Serial.println("Trying to reconnect to MQTT Server...");
      mqtt_error = connectBroker();
    }
  }
  else
  {
    // Try to reconnect to AP
    Serial.println("Trying to reconnect to AP...");
    wifi_error = connectAP();
  }

  Serial.println("");
  togglePin(BOARD_LED_PIN);
  delay(15000);   // Delay 10 seconds before next loop
}