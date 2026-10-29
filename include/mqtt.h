#ifndef MQTT_H
#define MQTT_H

void init_mqtt();
void mqtt_publish_data(const char* topic,int payloadlen,const void* payload);

#endif
