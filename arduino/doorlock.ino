#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include <AzureIoTHub.h>
#include <AzureIoTProtocol_MQTT.h>
#include <AzureIoTUtility.h>

// Define your Wi-Fi credentials if you're using Wi-Fi
const char* ssid = "YourWiFiSSID";
const char* password = "YourWiFiPassword";

// Azure IoT Hub settings
static const char* connectionString = "Your Device Connection String";

// Servo motor setup
Servo doorLockServo;
int servoLockedPosition = 0;    // Angle to lock the door
int servoUnlockedPosition = 90; // Angle to unlock the door

// Azure IoT client
IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;

// Callback for receiving cloud-to-device messages
static IOTHUBMESSAGE_DISPOSITION_RESULT ReceiveMessageCallback(
    IOTHUB_MESSAGE_HANDLE message,
    void* userContextCallback)
{
    const char* messageBody;
    if (IoTHubMessage_GetByteArray(message, (const unsigned char**)&messageBody) == IOTHUB_MESSAGE_OK)
    {
        // Check the message content to determine whether to lock or unlock the door
        if (strcmp(messageBody, "lock") == 0)
        {
            doorLockServo.write(servoLockedPosition);
        }
        else if (strcmp(messageBody, "unlock") == 0)
        {
            doorLockServo.write(servoUnlockedPosition);
        }
    }
    return IOTHUBMESSAGE_ACCEPTED;
}

void setup()
{
    // Initialize the servo motor
    doorLockServo.attach(9); // Connect the servo to pin 9
    doorLockServo.write(servoLockedPosition); // Initially lock the door

    // Initialize Serial communication
    Serial.begin(115200);

    // Connect to Wi-Fi (if using Wi-Fi)
    // WiFi.begin(ssid, password);
    // while (WiFi.status() != WL_CONNECTED)
    // {
    //     delay(1000);
    //     Serial.println("Connecting to WiFi...");
    // }
    // Serial.println("Connected to WiFi");

    // Initialize Azure IoT client
    iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, MQTT_Protocol);
    if (iotHubClientHandle == NULL)
    {
        Serial.println("Failed to create IoT Hub client");
        while (1);
    }
    IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, ReceiveMessageCallback, NULL);
}

void loop()
{
    // Run the Azure IoT client
    IoTHubClient_LL_DoWork(iotHubClientHandle);

    // Your other loop code can go here
}
