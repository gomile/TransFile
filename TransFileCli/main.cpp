#include "transfilecli.h"

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::cout << "usage: " << argv[0] << " serveraddress filename" << std::endl;
        return 0;
    }

    TransFileCli objTrans(argv[1], argv[2]);
    if (!objTrans.InitNetWork())
    {
        std::cout << objTrans.GetErrorMsg().c_str() << std::endl;
        return 0;
    }

    if (!objTrans.SendAndRecvFile())
    {
        std::cout << objTrans.GetErrorMsg().c_str() << std::endl;
        return 0;
    }

    std::cout << "done" << std::endl;
    return 0;
}
