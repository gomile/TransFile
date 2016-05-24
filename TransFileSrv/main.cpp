#include "transfilesrv.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "usage: " << argv[0] << "address" << std::endl;
        return 0;
    }

    transFileSrv objSrv(argv[1]);

    if (!objSrv.InitNetWork())
    {
        std::cout << objSrv.GetErrorMsg().c_str() << std::endl;
        return 0;
    }

    if (!objSrv.StartWork())
    {
        std::cout << objSrv.GetErrorMsg().c_str() << std::endl;
        return 0;
    }

    return 0;
}
