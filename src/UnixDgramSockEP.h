#pragma once

#include "ISockEP.h"

class UnixDgramSockEP : public ISockEP
{
public:
    UnixDgramSockEP() {};

}

class UnixStreamSockEP : public ISockEP
{
public:
    UnixStreamSockEP() {};

}

class TCPSockEP : public ISockEP
{
public:
    TCPSockEP() {};

}

class UDPSockEP : public ISockEP
{
public:
    UDPSockEP() {};

}
