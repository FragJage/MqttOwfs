#include<iostream>

#include "MqttClient.h"

using namespace std;

int MqttClient::m_NbInstances = 0;

MqttClient::MqttClient() : m_Server("127.0.0.1"), m_Port(1883), m_MainTopic(), m_KeepAlive(300), m_MessaseCallback(nullptr)
{
	if(m_NbInstances++==0)
		mosqpp::lib_init();
}

MqttClient::~MqttClient()
{
	if (--m_NbInstances==0)
		mosqpp::lib_cleanup();
}

void MqttClient::SetServer(std::string server, int port)
{
	m_Server = server;
	m_Port = port;
}

void MqttClient::SetMainTopic(string mainTopic)
{
    m_MainTopic = mainTopic;
    if(m_MainTopic.back() != '/') m_MainTopic.append("/");
}

void MqttClient::SetMessageCallback(IMqttMessage* messageCallback)
{
	m_MessaseCallback = messageCallback;
}

string MqttClient::GetMainTopic()
{
    return m_MainTopic;
}

void MqttClient::SetKeepAlive(int keepalive)
{
    m_KeepAlive = keepalive;
}

int MqttClient::GetKeepAlive()
{
    return m_KeepAlive;
}

int MqttClient::Open()
{
    return connect(m_Server.c_str(), m_Port, m_KeepAlive);
}

int MqttClient::Close()
{
    return disconnect();
}

int MqttClient::Publish(std::string sensor, std::string value)
{
    string topic = m_MainTopic+sensor;

    return publish(nullptr, topic.c_str(), value.length(), value.c_str());
}

int MqttClient::Subscribe(std::string topic)
{
	return subscribe(nullptr, topic.c_str());
}

int MqttClient::Loop(int timeout)
{
    int ret;

    ret = loop(timeout, 5);
	if(ret) reconnect();
	return ret;
}

string MqttClient::GetErrorMessage(int errorNo)
{
	return string(mosqpp::strerror(errorNo));
}

void MqttClient::on_message(const struct mosquitto_message* msg)
{
	string smsg = string((char *)msg->payload, msg->payloadlen);
	if (m_MessaseCallback != nullptr) m_MessaseCallback->on_message(msg->topic, smsg);
	return;
}