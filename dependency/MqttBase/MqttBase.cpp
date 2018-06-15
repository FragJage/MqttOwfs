#include "MqttBase.h"

using namespace std;

MqttBase::MqttBase() : m_Server("tcp://localhost:1883"), m_Id(""), m_MainTopic(""), m_KeepAlive(300), m_Timeout(5), m_MqttClient(nullptr)
{
}

MqttBase::~MqttBase()
{
	Disconnect();
}

void MqttBase::SetServer(const string& server, const string& id)
{
	m_Server = server;
	m_Id = id;
}

void MqttBase::SetMainTopic(const string& mainTopic)
{
    m_MainTopic = mainTopic;
    if(m_MainTopic.back() != '/') m_MainTopic.append("/");
}

string MqttBase::GetMainTopic()
{
    return m_MainTopic;
}

void MqttBase::SetKeepAlive(int keepalive)
{
    m_KeepAlive = keepalive;
}

void MqttBase::SetTimeout(int timeout)
{
	m_Timeout = timeout;
}

int MqttBase::GetKeepAlive()
{
    return m_KeepAlive;
}

void MqttBase::Connect()
{
	if (m_MqttClient!=nullptr) Disconnect();

	m_MqttClient = new mqtt::client(m_Server, m_Id);
	m_MqttClient->set_callback(*this);

	mqtt::connect_options connOpts;
	connOpts.set_automatic_reconnect(true);
	connOpts.set_clean_session(true);
	connOpts.set_connection_timeout(m_Timeout);
	connOpts.set_connect_timeout(m_Timeout);
	connOpts.set_keep_alive_interval(m_KeepAlive);

	m_MqttClient->connect(connOpts);
}

void MqttBase::Disconnect()
{
	if (m_MqttClient==nullptr) return;

	if(m_MqttClient->is_connected()) 
		m_MqttClient->disconnect();

	delete m_MqttClient;
	m_MqttClient = nullptr;
}

void MqttBase::Publish(const string& sensor, const string& value)
{
	if (m_MqttClient==nullptr) Connect();

    string topic = m_MainTopic+sensor;
	m_MqttClient->publish(topic, value.c_str(), value.length());
}

void MqttBase::Subscribe(const string& topic, int qos)
{
	if (m_MqttClient == nullptr) Connect();

	m_MqttClient->subscribe(topic, qos);
}

void MqttBase::Unubscribe(const std::string& topic)
{
	if (m_MqttClient == nullptr) return;
	m_MqttClient->unsubscribe(topic);
}

void MqttBase::message_arrived(mqtt::const_message_ptr msg)
{
	on_message(msg->get_topic(), msg->get_payload_str());
	return;
}