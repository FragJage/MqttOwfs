#include <iostream>
#include <exception>
#include "MqttOwfs.h"


using namespace std;

int main(int argc, char* argv[])
{
    int res = 0;

    try
    {
        MqttOwfs mqttOwfs;

        Service* pService = Service::Create("MqttOwfs", "Mqtt protocol bridge for Owfs", &mqttOwfs);
        pService->SetVersion("1.1");
        res = pService->Start(argc, argv);
        Service::Destroy();
    }
    catch(const exception &e)
    {
        std::cout << e.what();
    }

	#if defined(_MSC_VER) && defined(DEBUG)
	cout << "Press any key..." << endl;
	getchar();
	#endif

    return res;
}

