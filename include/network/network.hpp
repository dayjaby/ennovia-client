#ifndef ENNOVIA_NETWORK_HPP
#define ENNOVIA_NETWORK_HPP

#include "network/connection.hpp"
#include "network/message.hpp"
<<<<<<< HEAD
=======
#include "network/server.hpp"
>>>>>>> a9d8ab817077a0990fd2bc0567d420b12405c664
#include "network/client.hpp"

namespace Ennovia {
    typedef boost::shared_ptr<Connection> ServerConnection;
    typedef boost::shared_ptr<Connection> ClientConnection;


}
#endif
