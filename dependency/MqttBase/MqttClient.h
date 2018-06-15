#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <string>
#include "MqttBase.h"

class MqttClient : public MqttBase
{
    public:
	class IMqttMessage;
	MqttClient();
	virtual ~MqttClient();
	void SetMessageCallback(IMqttMessage* messageCallback);
	void on_message(const std::string& topic, const std::string& message);

    private:
	IMqttMessage* m_MessaseCallback;
};

class MqttClient::IMqttMessage
{
public:
	virtual ~IMqttMessage() {};
	virtual void on_message(const std::string& topic, const std::string& message) = 0;
};
#endif // MQTTCLIENT_H
