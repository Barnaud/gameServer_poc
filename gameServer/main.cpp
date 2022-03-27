#include <iostream>
#include "udp_server.h"
#include "Constants.h"

int main()
{
    std::cout << "Starting gameserver" << std::endl;
    try {
        udp_server my_server(server_port);
        my_server.start_listening();
        

    }
    catch (std::exception& error) {
        std::cerr << "Error: " << error.what() << std::endl;

    }

}

