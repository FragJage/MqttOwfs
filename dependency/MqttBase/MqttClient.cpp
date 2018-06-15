#include "MqttClient.h"

using namespace std;

MqttClient::MqttClient()
{
	m_MessaseCallback = nullptr;
}

MqttClient::~MqttClient()
{
}

void MqttClient::SetMessageCallback(IMqttMessage* messageCallback)
{
	m_MessaseCallback = messageCallback;
}

void MqttClient::on_message(const std::string& topic, const std::string& message)
{
	if (m_MessaseCallback == nullptr) return;
	m_MessaseCallback->on_message(topic, message);
	return;
}