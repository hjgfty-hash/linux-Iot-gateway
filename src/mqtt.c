#include "mqtt.h"
#include <mosquitto.h>
#include <stdio.h>

struct mosquitto *g_mosq = NULL;

void init_mqtt() {
    mosquitto_lib_init();
    g_mosq = mosquitto_new(NULL, true, NULL);
    if (g_mosq != NULL) {
        int res = mosquitto_connect(g_mosq, "127.0.0.1", 1883, 60);
        if (res == MOSQ_ERR_SUCCESS) {
            mosquitto_loop_start(g_mosq);
            printf("[MQTT] connect success\n");
        } else {
            printf("[MQTT] connect failure\n");
        }
    }
}

void mqtt_publish_data(const char* topic, int payloadlen, const void* payload) {
    if (g_mosq != NULL) {
        mosquitto_publish(g_mosq, NULL, topic, payloadlen, payload, 0, false);
    }
}
