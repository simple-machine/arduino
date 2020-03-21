typedef enum {
  ERROR_OK,
  ERROR_REQUEST,
  ERROR_FATAL, 
} error_t;

typedef enum {
  STATE_START,
  STATE_MAGIC,
  STATE_VERSION,
  STATE_CONFIRMATION,
  STATE_NORMAL,
} state_t;

const uint16_t VERSION = 0;
const char MAGIC_BYTES[4] = { 0x73, 0x6D, 0x6F, 0x76 };

state_t state = STATE_START;
error_t error = ERROR_OK;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  char buf[4];
}

void loop() {
  if (error == ERROR_FATAL) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
  } else {
    switch (state) {
      case STATE_START:
        char buf_magic[4];
        int r = Serial.readBytes(buf_magic, 4);
        Serial.write(buf_magic, r);
        Serial.write(MAGIC_BYTES, 4);
        if (r == 0) return;
        if (r != 4 || !strncmp(MAGIC_BYTES, buf_magic, 4)) {
          error = ERROR_FATAL;
          return;
        }
        state = STATE_START;
      case STATE_MAGIC:
        int w_m = Serial.write(MAGIC_BYTES, 4);
        if (w_m != 4) {
          error = ERROR_FATAL;
          return;
        }
        state = STATE_MAGIC;
      case STATE_VERSION:
        char buf_version[] = { (VERSION >> 8) & 0xFF, VERSION & 0xFF };
        int w_v = Serial.write(MAGIC_BYTES, 4);
        if (w_v != 4) {
          error = ERROR_FATAL;
          return;
        }
        state = STATE_VERSION;
      case STATE_CONFIRMATION:
        char conf = Serial.read();
        if (conf == -1) return;
        if (conf != 0x00) {
          error = ERROR_FATAL;
          return;
        }
      case STATE_NORMAL:
        char buf_command[8];
        int r_command = Serial.readBytes(buf_command, 8);
        if (r_command == 0) return;
        switch (buf_command[0]) {
          case 0x00:
            Serial.write(0x00);
            digitalWrite(LED_BUILTIN, LOW);
            break;
          case 0x01:
            if (r_command != 3) {
              Serial.write(0x01);
              digitalWrite(LED_BUILTIN, HIGH);
            } else {
              int power = ((int) buf_command[1]) << 8 | buf_command[2];
              Serial.write(0x00);
              digitalWrite(LED_BUILTIN, LOW);
            }
            break;
          default:
            digitalWrite(LED_BUILTIN, HIGH);
        }
    }
  }
}
