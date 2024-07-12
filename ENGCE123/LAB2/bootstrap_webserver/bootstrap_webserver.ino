#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// WiFi credentials
const char* ssid = "papon";
const char* password = "0633468203";

// Create an instance of the server
ESP8266WebServer server(80);

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200, 60000); // NTP Server, Time offset in seconds (7 hours), update interval

// HTML page
const char* htmlPage = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Time Control</title>
    <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
    <style>
        .time-table, .time-setting-form {
            margin-top: 20px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Alarm to Set Time for Device to Open and Close</h1>

        <!-- Table for Time Control -->
        <table class="table table-bordered time-table">
            <thead>
                <tr>
                    <th>Order</th>
                    <th>Time</th>
                    <th>Release (sec)</th>
                    <th>Active</th>
                    <th>Action</th>
                </tr>
            </thead>
            <tbody id="timeTableBody">
            </tbody>
        </table>

        <!-- Form for Setting Time -->
        <form class="time-setting-form" id="timeSettingForm">
            <div class="form-group">
                <label for="orderNumber">Order Number</label>
                <div class="input-group">
                    <div class="input-group-prepend">
                        <button class="btn btn-outline-secondary" type="button" id="decreaseOrderBtn">-</button>
                    </div>
                    <input type="number" class="form-control" id="orderNumber" value="1">
                    <div class="input-group-append">
                        <button class="btn btn-outline-secondary" type="button" id="increaseOrderBtn">+</button>
                    </div>
                </div>
            </div>
            <div class="form-group">
                <label for="time">Time</label>
                <input type="time" class="form-control" id="time">
            </div>
            <div class="form-group">
                <label for="release">Release (sec)</label>
                <div class="input-group">
                    <div class="input-group-prepend">
                        <button class="btn btn-outline-secondary" type="button" id="decreaseReleaseBtn">-</button>
                    </div>
                    <input type="number" class="form-control" id="release" value="10">
                    <div class="input-group-append">
                        <button class="btn btn-outline-secondary" type="button" id="increaseReleaseBtn">+</button>
                    </div>
                </div>
            </div>
            <div class="form-group">
                <label for="active">Active</label>
                <select class="form-control" id="active">
                    <option>Yes</option>
                    <option>No</option>
                </select>
            </div>
            <button type="button" class="btn btn-primary" id="setTimeBtn">Set Time</button>
        </form>
    </div>

    <script src="https://code.jquery.com/jquery-3.5.1.min.js"></script>
    <script>
        document.getElementById('decreaseOrderBtn').addEventListener('click', function() {
            var orderInput = document.getElementById('orderNumber');
            var orderValue = parseInt(orderInput.value, 10);
            if (orderValue > 1) {
                orderInput.value = orderValue - 1;
            }
        });

        document.getElementById('increaseOrderBtn').addEventListener('click', function() {
            var orderInput = document.getElementById('orderNumber');
            var orderValue = parseInt(orderInput.value, 10);
            orderInput.value = orderValue + 1;
        });

        document.getElementById('decreaseReleaseBtn').addEventListener('click', function() {
            var releaseInput = document.getElementById('release');
            var releaseValue = parseInt(releaseInput.value, 10);
            if (releaseValue > 0) {
                releaseInput.value = releaseValue - 1;
            }
        });

        document.getElementById('increaseReleaseBtn').addEventListener('click', function() {
            var releaseInput = document.getElementById('release');
            var releaseValue = parseInt(releaseInput.value, 10);
            releaseInput.value = releaseValue + 1;
        });

        document.getElementById('setTimeBtn').addEventListener('click', function() {
            var orderNumber = document.getElementById('orderNumber').value;
            var time = document.getElementById('time').value;
            var release = document.getElementById('release').value;
            var active = document.getElementById('active').value;

            var tableBody = document.getElementById('timeTableBody');
            var newRow = document.createElement('tr');
            newRow.innerHTML = '<td>' + orderNumber + '</td>' +
                               '<td>' + time + '</td>' +
                               '<td>' + release + '</td>' +
                               '<td>' + active + '</td>' +
                               '<td><button class="btn btn-danger btn-sm deleteBtn">Delete</button></td>';
            tableBody.appendChild(newRow);

            // Add event listener to the new delete button
            newRow.querySelector('.deleteBtn').addEventListener('click', function() {
                this.closest('tr').remove();
            });

            // Send data to ESP8266
            $.post('/set_time', { order: orderNumber, time: time, release: release, active: active });
        });

        // Add event listeners to existing delete buttons
        document.querySelectorAll('.deleteBtn').forEach(function(button) {
            button.addEventListener('click', function() {
                this.closest('tr').remove();
            });
        });
    </script>
</body>
</html>
)";

struct TimerSetting {
  int order;
  int hour;
  int minute;
  int release;
  bool active;
};

std::vector<TimerSetting> timerSettings;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();

  server.on("/", []() {
    server.send(200, "text/html", htmlPage);
  });

  server.on("/set_time", HTTP_POST, []() {
    if (server.hasArg("order") && server.hasArg("time") && server.hasArg("release") && server.hasArg("active")) {
      TimerSetting setting;
      setting.order = server.arg("order").toInt();
      String time = server.arg("time");
      setting.hour = time.substring(0, 2).toInt();
      setting.minute = time.substring(3, 5).toInt();
      setting.release = server.arg("release").toInt();
      setting.active = (server.arg("active") == "Yes");

      Serial.println("Order: " + String(setting.order));
      Serial.println("Time: " + String(setting.hour) + ":" + String(setting.minute));
      Serial.println("Release: " + String(setting.release));
      Serial.println("Active: " + String(setting.active ? "Yes" : "No"));

      timerSettings.push_back(setting);

      server.send(200, "text/plain", "Time set");
    } else {
      server.send(400, "text/plain", "Bad Request");
    }
  });

  server.begin();
  Serial.println("Server started");
}

void loop() {
  server.handleClient();
  timeClient.update();
  
  unsigned long currentMillis = millis();
  static unsigned long previousMillis = 0;

  if (currentMillis - previousMillis >= 60000) {
    previousMillis = currentMillis;

    int currentHour = timeClient.getHours();
    int currentMinute = timeClient.getMinutes();

    for (const auto& setting : timerSettings)
        {
        if (setting.active && setting.hour == currentHour && setting.minute == currentMinute) {
            digitalWrite(LED_BUILTIN, LOW);
            delay(setting.release * 1000);
            digitalWrite(LED_BUILTIN, HIGH);
        }
    }
  }
}
