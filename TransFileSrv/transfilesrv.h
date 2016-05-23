#ifndef TRANSFILESRV_H
#define TRANSFILESRV_H

#include <iostream>
#include <vector>
#include <arpa/inet.h>
#include <sys/types.h>


class transFileSrv
{
public:
    transFileSrv(std::string strAddr) : m_strAddr(strAddr), m_nPort(5050)
    {
        m_vectClients.assign(FD_SETSIZE, -1);
    }

public:
    bool InitNetWork();
    bool StartWork();

private:
    std::string ErrorMsg(std::initializer_list<std::string>& li);

    bool FillSockInfo(struct sockaddr_in& objSockInfo);
    bool BindAndListen(struct sockaddr_in& objSockInfo);

    void clear()
    {
        close(m_nListenFd);
    }


private:
    std::vector<int> m_vectClients; //all clients fd
    std::string      m_strAddr;
    int              m_nPort;

    int              m_nListenFd;

    std::string      m_strErroMsg;

};

#endif // TRANSFILESRV_H
