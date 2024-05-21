#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "IoT";
const char* password = "82253511";

const char* PARAM_INPUT_1 = "Input";
const char* PARAM_INPUT_2 = "Input";

int pins[] = {4,5,16};

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
  <head>
    <title>ESP Web Server</title>
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <link
      rel="stylesheet"
      href="https://cdn.jsdelivr.net/npm/bootstrap@4.0.0/dist/css/bootstrap.min.css"
      integrity="sha384-Gn5384xqQ1aoWXA+058RXPxPg6fy4IWvTNh0E263XmFcJlSAwiGgFAW/dAiS6JXm"
      crossorigin="anonymous"
    />
    <link rel="icon" href="data:," />
    <script src="https://code.jquery.com/jquery-3.5.1.min.js"></script>
    <style>
      html {
        font-family: Arial;
        display: inline-block;
        text-align: center;
      }
      h2 {
        font-size: 3rem;
      }
      p {
        font-size: 3rem;
      }
      body {
        max-width: 600px;
        margin: 0px auto;
        padding-bottom: 25px;
      }
      .switch {
        position: relative;
        display: inline-block;
        width: 120px;
        height: 68px;
      }
      .switch input {
        display: none;
      }
      .slider {
        position: absolute;
        top: 0;
        left: 0;
        right: 0;
        bottom: 0;
        background-color: #ccc;
        border-radius: 6px;
      }
      .slider:before {
        position: absolute;
        content: "";
        height: 52px;
        width: 52px;
        left: 8px;
        bottom: 8px;
        background-color: #fff;
        -webkit-transition: 0.4s;
        transition: 0.4s;
        border-radius: 3px;
      }
      input:checked + .slider {
        background-color: #b30000;
      }
      input:checked + .slider:before {
        -webkit-transform: translateX(52px);
        -ms-transform: translateX(52px);
        transform: translateX(52px);
      }
    </style>
  </head>
  <body>
    <h2>ESP Web Server</h2>
    %BUTTONPLACEHOLDER%
    <script>
      var myPollingInterval = setInterval(function () {
        var xhr = new XMLHttpRequest();
        if (element.checked) {
          xhr.open("GET", "/update", true);
        } else {
          xhr.open("GET", "/update", true);
        }
        xhr.send();
      }, 2000);

      clearInterval(myPollingInterval); //stop the polling
    </script>
    <script
      src="https://cdn.jsdelivr.net/npm/popper.js@1.12.9/dist/umd/popper.min.js"
      integrity="sha384-ApNbgh9B+Y1QKtv3Rn7W3mgPxhU9K/ScQsAP7hUibX39j7fakFPskvXusvfa0b4Q"
      crossorigin="anonymous"
    ></script>
    <script
      src="https://cdn.jsdelivr.net/npm/bootstrap@4.0.0/dist/js/bootstrap.min.js"
      integrity="sha384-JZR6Spejh4U02d8jOt6vLEHfe/JQGiRRSQQxSfFWpi1MquVdAyjUar5+76PVCmYl"
      crossorigin="anonymous"
    ></script>
  </body>
</html>
)rawliteral";

int outputState(int output){
  int input = analogRead(output)* (5.0 / 1023.0);
  if(input){
    Serial.print("GPIO: ");
    Serial.print(input);
    return input;
  }
  else {
    return 0;
  }
}

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if(var == "BUTTONPLACEHOLDER"){
    String buttons = "";
    buttons += "<h4>Input - GPIO 5</h4><h3>" + String(outputState(5)) + "</h3>";
    buttons += "<h4>Input - GPIO 4</h4><h3>" + String(outputState(4)) + "</h3>";
    buttons += "<h4>Input - GPIO 16</h4><h3>" + String(outputState(16)) + "</h3>";
    return buttons;
  }
  return String();
}



void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  pinMode(5, INPUT);
  pinMode(4, INPUT);
  pinMode(16, INPUT);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
    for (size_t i = 0; i < sizeof pins; i++)
    {
      Serial.print("GPIO: ");
      Serial.print(String(outputState(pins[i])));
    }
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/update
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    for (int pin: pins)
    {
      Serial.print("GPIO: ");
      Serial.println(String(outputState(pin)));
    }
    request->send(200, "text/plain", "OK");
  });

  // Start server
  server.begin();
}

void loop() {

}
