#include <iostream>
#include "UdpServer.h"
#include "Constants.h"




int main()
{

    std::cout << "Starting gameserver" << std::endl;
    try {
        UdpServer myUdpServer(server_port);
        myUdpServer.startListening();
        

    }
    catch (std::exception& error) {
        std::cerr << "Error: " << error.what() << std::endl;

    }

}

