#include <iostream>
#include "udp_server.h"
#include "Constants.h"




int main()
{

    std::cout << "Starting gameserver" << std::endl;
    try {
        udp_server my_udp_server(server_port);
        my_udp_server.start_listening();
        

    }
    catch (std::exception& error) {
        std::cerr << "Error: " << error.what() << std::endl;

    }

}

