const uint16_t VERSION = 0;
const char MAGIC_BYTES[4] = { 0x73, 0x6D, 0x6F, 0x76 };

const int MOTOR_PIN = 3; // This is specific to the Arduino UNO, change according to the platform

bool init_error = false; // Was there a fatal error
uint16_t last_send; // When was the last heartbeat sent?

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);
  Serial.begin(115200);

  digitalWrite(LED_BUILTIN, LOW);

  while (!Serial) delay(1); // Wait for interface

  // Initialization
  char buf_magic[4];
  while (Serial.available() < 4) delay(1);
  int r = Serial.readBytes(buf_magic, 4);
  if (r != 4 || strncmp(MAGIC_BYTES, buf_magic, 4) != 0) {
    init_error = true;
    return;
  }
  int w_m = Serial.write(MAGIC_BYTES, 4);
  if (w_m != 4) {
    init_error = true;
    return;
  }
  char buf_version[] = { (VERSION >> 8) & 0xFF, VERSION & 0xFF };
  int w_v = Serial.write(buf_version, 2);
  if (w_v != 2) {
    init_error = true;
    return;
  }

  while (Serial.available() == 0) delay(1);
  char conf = Serial.read();
  if (conf != 0x00) {
    init_error = true;
    return;
  }
  last_send = millis();
}

void loop() {
  if (init_error) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
  } else if (Serial.available() > 0) {
    switch (Serial.read()) {
      case 0x00:
        Serial.write(0x00);
        break;
      case 0x01:
        char buf_command[2];
        int r_command = Serial.readBytes(buf_command, 2);
        if (r_command != 2) {
          Serial.write(0x01);
          digitalWrite(LED_BUILTIN, HIGH);
        } else {
          int power = ((int) buf_command[0]) << 8 | buf_command[1];
          analogWrite(MOTOR_PIN, map(power, 0, (2 << 16) - 1, 0, 255));
          Serial.write(0x00);
          digitalWrite(LED_BUILTIN, LOW);
        }
        break;
      default:
        Serial.write(0xFF);
        digitalWrite(LED_BUILTIN, HIGH);
    }
    last_send = millis();
  } else if (((uint16_t)millis()) - last_send >= 1000) {
    init_error = true;
  } else {
    delay(1);
  }
}
