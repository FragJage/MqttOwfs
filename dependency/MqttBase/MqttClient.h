#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <string>
#include "mosquittopp.h"

#if LIBMOSQUITTO_MAJOR > 0
	#define MOSQUITTO_NS mosqpp
#else
	#define MOSQUITTO_NS mosquittopp
#endif

class MqttClient : public MOSQUITTO_NS::mosquittopp
{
    public:
	class IMqttMessage;
	MqttClient();
	~MqttClient();

	void SetServer(const std::string& server, int port);
	void SetMainTopic(const std::string& mainTopic);
	void SetKeepAlive(int keepalive);
	void SetMessageCallback(IMqttMessage* messageCallback);
	std::string GetMainTopic();
	int GetKeepAlive();

	int Open();
	int Close();
	int Publish(const std::string& sensor, const std::string& value);
	int Subscribe(const std::string& topic);
	int Loop(int timeout);
	std::string GetErrorMessage(int errorNo);

    protected:

    private:
	static int m_NbInstances;
	void on_message(const struct mosquitto_message* msg);
	std::string m_Server;
        int m_Port;
        std::string m_MainTopic;
        std::string m_LastError;
        int m_KeepAlive;
	IMqttMessage* m_MessaseCallback;
};

class MqttClient::IMqttMessage
{
public:
	virtual void on_message(const std::string& topic, const std::string& message) = 0;
};
#endif // MQTTCLIENT_H
