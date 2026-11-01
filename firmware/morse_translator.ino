/**
 * ESP32-Based Assistive Morse Code Communicator
 * Author: Kalinga Puneeth Kumar
 * Institution: Amrita Vishwa Vidyapeetham, Coimbatore
 * Description: Decodes tactile button presses into Morse code dots/dashes,
 *              translates them to alphanumeric characters, and broadcasts
 *              real-time text via WebSockets to a web dashboard.
 */

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Wi-Fi Configuration
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Hardware Pin Definitions
const int BUTTON_PIN = 4;   // Input tactile button (active-low with pull-up)
const int BUZZER_PIN = 18;  // Audio feedback buzzer
const int LED_PIN = 2;      // Visual status LED

// Timing Thresholds (in milliseconds)
const unsigned long DOT_DURATION_MAX = 250;    // <= 250ms is Dot
const unsigned long DASH_DURATION_MAX = 800;   // > 250ms and <= 800ms is Dash
const unsigned long CHAR_TIMEOUT = 1200;       // Pause > 1200ms triggers char decode
const unsigned long WORD_TIMEOUT = 3000;       // Pause > 3000ms adds space

// State tracking variables
unsigned long pressStartTime = 0;
unsigned long releaseStartTime = 0;
bool isPressed = false;
String currentMorse = "";
String decodedMessage = "";

// Web Server & WebSocket instances
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Morse code lookup table
struct MorseMap {
  const char* code;
  char letter;
};

const MorseMap morseDictionary[] = {
  {".-", 'A'},   {"-...", 'B'}, {"-.-.", 'C'}, {"-..", 'D'},  {".", 'E'},
  {"..-.", 'F'}, {"--.", 'G'},  {"....", 'H'}, {"..", 'I'},   {".---", 'J'},
  {"-.-", 'K'},  {".-..", 'L'}, {"--", 'M'},   {"-.", 'N'},   {"---", 'O'},
  {".--.", 'P'}, {"--.-", 'Q'}, {".-.", 'R'},  {"...", 'S'},  {"-", 'T'},
  {"..-", 'U'},  {"...-", 'V'}, {".--", 'W'},  {"-..-", 'X'}, {"-.--", 'Y'},
  {"--..", 'Z'}, {"-----", '0'}, {".----", '1'}, {"..---", '2'}, {"...--", '3'},
  {"....-", '4'}, {".....", '5'}, {"-....", '6'}, {"--...", '7'}, {"---..", '8'},
  {"----.", '9'}
};

char decodeMorse(String morse) {
  for (unsigned int i = 0; i < sizeof(morseDictionary) / sizeof(MorseMap); i++) {
    if (morse.equals(morseDictionary[i].code)) {
      return morseDictionary[i].letter;
    }
  }
  return '?'; // Unknown character
}

void broadcastUpdate(String type, String value) {
  String payload = "{\"type\":\"" + type + "\",\"value\":\"" + value + "\"}";
  ws.textAll(payload);
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    client->text("{\"type\":\"sync\",\"message\":\"" + decodedMessage + "\"}");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.begin();
}

void loop() {
  ws.cleanupClients();
  int buttonState = digitalRead(BUTTON_PIN);

  // Button pressed (active-low)
  if (buttonState == LOW && !isPressed) {
    isPressed = true;
    pressStartTime = millis();
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
  }
  // Button released
  else if (buttonState == HIGH && isPressed) {
    isPressed = false;
    unsigned long duration = millis() - pressStartTime;
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);

    if (duration <= DOT_DURATION_MAX) {
      currentMorse += ".";
    } else if (duration <= DASH_DURATION_MAX) {
      currentMorse += "-";
    }
    releaseStartTime = millis();
    broadcastUpdate("morse", currentMorse);
  }

  // Inter-character timeout check
  if (!isPressed && currentMorse.length() > 0 && (millis() - releaseStartTime > CHAR_TIMEOUT)) {
    char letter = decodeMorse(currentMorse);
    decodedMessage += letter;
    broadcastUpdate("char", String(letter));
    broadcastUpdate("message", decodedMessage);
    currentMorse = "";
    broadcastUpdate("morse", "");
  }

  // Inter-word space check
  if (!isPressed && decodedMessage.length() > 0 && !decodedMessage.endsWith(" ") && (millis() - releaseStartTime > WORD_TIMEOUT)) {
    decodedMessage += " ";
    broadcastUpdate("message", decodedMessage);
  }
}
