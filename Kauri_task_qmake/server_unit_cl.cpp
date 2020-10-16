#include "server_unit_cl.h"


TCP_Init_Data::TCP_Init_Data(int Af, int Type, int Protocol, int BufSize, const char *netPort, const char *nodeName)
{
    af=Af;
    type=Type;
    protocol=Protocol;
    BufSz=BufSize;
    NetPort=netPort;
    NodeName=nodeName;
}
