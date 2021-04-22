AsyncWebServer server(SERVER_PORT);
IPAddress serverIPAddress;

void initializeServer(){
  WiFi.mode(WIFI_MODE_APSTA);

  WiFi.softAP(AP_SSID, AP_PASSWORD);
  serverIPAddress = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(serverIPAddress);
}


void startServer(){
    server.begin();
    
    
}
