#ifndef TRANSFILECLI_H
#define TRANSFILECLI_H
#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>
#include <initializer_list>

class TransFileCli
{
public:
    TransFileCli(std::string strAddr, std::string strFileName) : m_strAddress(strAddr), m_strFileName(strFileName), m_nPort(5050)
    {
        registerSignal();
    }

    TransFileCli();
    ~TransFileCli()
    {
        clear();
    }

    bool InitNetWork();

    bool SendAndRecvFile();

    const std::string& GetErrorMsg() const
    {
        return m_strErroMsg;
    }

private:

    bool OpenFile();
    bool FillSockInfo(struct sockaddr_in& objSockInfo);
    bool Connect(struct sockaddr_in& objSockInfo);

    bool CreateFile();

    bool registerSignal();

    std::string ErrorMsg(std::initializer_list<std::string> li);

    void clear()
    {
        close(m_nFilefd);
        close(m_nSockfd);
        close(m_nWritefd);
    }

private:
    union funcMem
    {
        void(*sig_handler)(int);
        void(TransFileCli::*sig_Classhandler)(int);
    };

private:
    std::string m_strAddress;
    std::string m_strFileName;
    int         m_nPort;

    int         m_nSockfd;
    int         m_nFilefd;
    int         m_nWritefd;
    funcMem     func;
    std::string m_strErroMsg;

};

#endif // TRANSFILECLI_H
