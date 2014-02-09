#include <iostream>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include "mayor.hpp"

using namespace Ennovia;

int main(int argc, char* argv[])
{
    Mayor& mayor = Mayor::get();
    mayor.run();
    return 0;
}

