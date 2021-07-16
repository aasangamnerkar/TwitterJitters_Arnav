SYSTEM_THREAD(ENABLED);

#include "MQTT.h"
#include "oled-wing-adafruit.h"

const size_t UART_TX_BUF_SIZE = 20;
bool firstConnected = false;
String topic = "twitter";

const BleUuid serviceUuid("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
const BleUuid rxUuid("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");
const BleUuid txUuid("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");

//set up BLE
BleCharacteristic txCharacteristic("tx", BleCharacteristicProperty::NOTIFY, txUuid, serviceUuid);
BleCharacteristic rxCharacteristic("rx", BleCharacteristicProperty::WRITE_WO_RSP, rxUuid, serviceUuid, onDataReceived, NULL);

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Received message on testTopic/wbk: \n");
    String message = "check twitter :)";
    
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;

    Serial.printf("%s",p);

    if (BLE.connected() == 1)
    {
      uint8_t txBuf[UART_TX_BUF_SIZE];
      message.toCharArray((char *)txBuf, message.length() + 1);
      txCharacteristic.setValue(txBuf, message.length() + 1);
    }
}

MQTT client("lab.thewcl.com", 1883, callback);
OledWingAdafruit display;


void printToDisplay(String output)
{
  //reset display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

      //output to display
  display.println(output);
  display.display();
}

void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context) {
    for (size_t i = 0; i < len; i++) {
        if ((char) data[i] == '1')
        {
          //printToDisplay("SLATT");
        }
        else if ((char) data[i] == '0')
        {
          //firstConnected = false;
        }
    }
}



// setup() runs once, when the device is first turned on.
void setup() {
  BLE.on();

  BLE.addCharacteristic(txCharacteristic);
  BLE.addCharacteristic(rxCharacteristic);

  BleAdvertisingData data;
  data.appendServiceUUID(serviceUuid);
  BLE.advertise(&data);


  Serial.begin(9600);

  display.setup();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("");
  display.display();

  client.connect(System.deviceID());
  if (client.isConnected())
  {
    client.subscribe(topic);
    client.publish(topic,"hello world");
  }
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  display.loop();

  client.connect(System.deviceID());
  client.subscribe(topic);
  if (client.isConnected())
  {
    client.loop();
  }
}