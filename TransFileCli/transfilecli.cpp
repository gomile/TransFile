#include "transfilecli.h"
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>

const int MAXLINE = 1024;

inline int max(int a, int b)
{
    return a > b ? a : b;
}

TransFileCli::TransFileCli()
{

}

std::string TransFileCli::ErrorMsg(std::initializer_list<std::string>& li)
{
    std::string strTemp;
    for (std::string& str : li)
        strTemp += str;
    return strTemp;
}

bool TransFileCli::FillSockInfo(struct sockaddr_in& objSockInfo)
{
    objSockInfo.sin_family = AF_INET;
    objSockInfo.sin_port = htons(m_nPort);
    if (inet_pton(AF_INET, m_strAddress.c_str(), &objSockInfo.sin_addr) < 0)
    {
        m_strErroMsg = ErrorMsg({"inet_pton error()", strerror(errno)});
        return false;
    }

    m_nSockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_nSockfd < 0)
    {
        m_strErroMsg = ErrorMsg({"socket()", strerror(errno)});
        return false;
    }

    return true;
}

bool TransFileCli::Connect(struct sockaddr_in& objSockInfo)
{
    if (connect(m_nSockfd, (sockaddr*)&objSockInfo, sizeof(sockaddr)) < 0)
    {
        m_strErroMsg = ErrorMsg({"connect() error", strerror(errno)});
        return false;
    }
}

bool TransFileCli::InitNetWork()
{
    sockaddr_in objSockInfo;

    if(!FillSockInfo(objSockInfo))
        return false;
    if (!Connect(objSockInfo))
        return false;
    if (!OpenFile())
        return false;
    return true;
}

bool TransFileCli::OpenFile()
{
    m_nFilefd = open(m_strFileName.c_str(), O_RDONLY);
    if (m_nFilefd < 0)
    {
        m_strErroMsg = ErrorMsg({"open() error", strerror(errno)});
        return false;
    }
    return true;
}

bool TransFileCli::CreateFile()
{
    m_nWritefd = open("tmpfile", O_RDWR | O_CREAT | O_EXCL);
    if (m_nWritefd < 0)
    {
        m_strErroMsg = ErrorMsg({"open() error", strerror(errno)});
        clear();
        return false;
    }
    return true;
}

bool TransFileCli::SendAndRecvFile()
{
    fd_set fdReadAble;
    FD_ZERO(&fdReadAble);
    char buff[MAXLINE];
    char readBuff[MAXLINE];

    if (!CreateFile())
        return false;

    int maxfd = max(m_nFilefd, m_nSockfd);
    while(1)
    {
        FD_SET(m_nFilefd, &fdReadAble);
        FD_SET(m_nSockfd, &fdReadAble);

        if (select(maxfd + 1, &fdReadAble, NULL, NULL, NULL) < 0)
        {
            m_strErroMsg = ErrorMsg({"select error", strerror(errno)});
            return false;
        }

        if (FD_ISSET(m_nFilefd, &fdReadAble))
        {
            int nread = read(m_nFilefd, buff, MAXLINE);
            if (nread < 0)
            {
                m_strErroMsg = ErrorMsg({"read() error", strerror(errno)});
                clear();
                return false;
            }
            if (nread == 0) //read over
                shutdown(m_nSockfd, SHUT_WR); // make sock be half
            if (nread > 0)
            {
                int nwrite = write(m_nSockfd, buff, nread);
                if (nwrite != nread)
                {
                    m_strErroMsg = ErrorMsg({"write() error", strerror(errno)});
                    clear();
                    return false;
                }
            }
        }

        if (FD_ISSET(m_nSockfd, &fdReadAble))
        {
            int nread = read(m_nSockfd, readBuff, MAXLINE);
            if (nread < 0)
            {
                m_strErroMsg = ErrorMsg({"read() error", strerror(errno)});
                clear();
                return false;
            }
            if (nread == 0) //server close
                break;

            int nwrite = write(m_nWritefd, readBuff, nread);
            if (nread != nwrite)
            {
                m_strErroMsg = ErrorMsg({"write() error", strerror(errno)});
                clear();
                return false;
            }
        }
    }

    clear();
    return true;
}
