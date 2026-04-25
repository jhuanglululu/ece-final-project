#ifndef IOT_SCORE_INO
#define IOT_SCORE_INO

/* 7 segment display */
const int STCP_PIN = 4; // brown IO4
const int SHCP_PIN = 5; // blue IO5
const int DS_PIN = 2;   // purple io 2

void init_iot_score();
void update_iot_score(uint32_t time_diff);
void set_score_display(int score);
void publish_score(int score);

#endif

#ifdef IOT_SCORE_IMPL
#ifndef IOT_SCORE_IMPL_GUARD
#define IOT_SCORE_IMPL_GUARD

#include <PubSubClient.h>
#include <WiFi.h>

const char *ssid = "Registered4OSU";
const char *password = "xv7bj5hhde59D5qD";

const char *mqtt_server = "broker.hivemq.com";
const char *unique_identifier = "team-2-snake-game-2360";
const char *score_topic = "team-2-snake-game-SCORE";

WiFiClient espClient;
PubSubClient client(espClient);

int score_display_value = 0;
uint32_t score_flash_timer = 0;
bool score_flash_tens = true;

int datArray[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};

void write_7seg_digit(int digit) {
    digit = constrain(digit, 0, 9);

    digitalWrite(STCP_PIN, LOW);
    shiftOut(DS_PIN, SHCP_PIN, MSBFIRST, datArray[digit]);
    digitalWrite(STCP_PIN, HIGH);
}

void setup_wifi() {
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi connected");
    Serial.println(WiFi.localIP());
}

void reconnect_mqtt() {
    if (!client.connected()) {
        Serial.print("Attempting MQTT connection...");

        if (client.connect(unique_identifier)) {
            Serial.println("connected");
        } else {
            Serial.print("failed, rc=");
            Serial.println(client.state());
        }
    }
}

void init_iot_score() {
    Serial.begin(115200);

    setup_wifi();

    client.setServer(mqtt_server, 1883);

    pinMode(STCP_PIN, OUTPUT);
    pinMode(SHCP_PIN, OUTPUT);
    pinMode(DS_PIN, OUTPUT);

    set_score_display(1);
}

void update_iot_score(uint32_t time_diff) {
    static uint32_t mqtt_retry_timer = 0;

    mqtt_retry_timer += time_diff;
    if (!client.connected() && mqtt_retry_timer >= 5000) {
        mqtt_retry_timer = 0;
        reconnect_mqtt();
    }

    if (client.connected()) {
        client.loop();
    }

    if (score_display_value < 10) {
        write_7seg_digit(score_display_value);
        return;
    }

    score_flash_timer += time_diff;

    if (score_flash_timer >= 500) {
        score_flash_timer = 0;
        score_flash_tens = !score_flash_tens;
    }

    if (score_flash_tens) {
        write_7seg_digit(score_display_value / 10);
    } else {
        write_7seg_digit(score_display_value % 10);
    }
}

void set_score_display(int score) {
    score_display_value = constrain(score, 0, 32);
    score_flash_timer = 0;
    score_flash_tens = true;

    if (score_display_value < 10) {
        write_7seg_digit(score_display_value);
    } else {
        write_7seg_digit(score_display_value / 10);
    }
}

void publish_score(int score) {
    char scoreString[8];
    itoa(score, scoreString, 10);

    Serial.print("Score: ");
    Serial.println(scoreString);

    if (client.connected()) {
        client.publish(score_topic, scoreString);
    }
}

#endif
#endif