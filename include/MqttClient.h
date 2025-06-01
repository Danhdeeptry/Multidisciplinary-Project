#pragma once
#include <WiFi.h>
#include <Arduino_MQTT_Client.h>
#include <ThingsBoard.h>
#include <ArduinoJson.h>
#include "Utils.h"
#include "Sensor.h"
#include "LightControl.h"

// WiFi/MQTT client objects
extern WiFiClient wifiClient;
extern Arduino_MQTT_Client mqttClient;
extern ThingsBoard tb;

// Attribute changed flag
extern volatile bool attributesChanged;

// RPC callback declarations
RPC_Response setLedSwitchState(const RPC_Data &data);
RPC_Response readSensorData(const RPC_Data &data);
RPC_Response setGrowLightState(const RPC_Data &data);
RPC_Response setAutoLightMode(const RPC_Data &data);

// Callback arrays
extern std::array<RPC_Callback, 4> callbacks;
extern Shared_Attribute_Callback attributes_callback;
extern Attribute_Request_Callback attribute_shared_request_callback;

// WiFi/MQTT init
void initWiFi();
bool reconnectWiFi();
void initMqtt();

// FreeRTOS task prototypes
void TbLoopTask(void *pvParameters);
void reconnectWiFiTask(void *pvParameters);
void connectThingsBoardTask(void *pvParameters);
void sendAttributesTask(void *pvParameters);
void sendTelemetryTask(void *pvParameters);