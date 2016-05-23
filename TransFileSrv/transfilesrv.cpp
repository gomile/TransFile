#include "transfilesrv.h"
#include <algorithm> // find function
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


const int MAXLINE = 1024;

std::string transFileSrv::ErrorMsg(std::initializer_list<std::string>& li)
{
    std::string strTemp;
    for (std::string& str : li)
        strTemp += str;
    return strTemp;
}

bool transFileSrv::FillSockInfo(struct sockaddr_in& objSockInfo)
{
    objSockInfo.sin_family = AF_INET;
    objSockInfo.sin_port = htons(m_nPort);
    if (inet_pton(AF_INET, m_strAddr.c_str(), &objSockInfo.sin_addr) < 0)
    {
        m_strErroMsg = ErrorMsg({"inet_pton error()", strerror(errno)});
        return false;
    }

    m_nListenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_nSockfd < 0)
    {
        m_strErroMsg = ErrorMsg({"socket()", strerror(errno)});
        clear();
        return false;
    }

    return true;
}

bool transFileSrv::BindAndListen(struct sockaddr_in& objSockInfo)
{
    if (bind(m_nListenFd, (sockaddr*)&objSockInfo, sizeof(sockaddr)) < 0)
    {
        m_strErroMsg = ErrorMsg({"bind()", strerror(errno)});
        clear();
        return false;
    }

    if (listen(m_nListenFd, 100) < 0)
    {
        m_strErroMsg = ErrorMsg({"listen()", strerror(errno)});
        clear();
        return false;
    }

    return true;
}

bool transFileSrv::InitNetWork()
{
    sockaddr_in objSockInfo;

    if(!FillSockInfo(objSockInfo))
        return false;
    if (!BindAndListen(objSockInfo))
        return false;
    return true;
}

bool transFileSrv::StartWork()
{
    fd_set rset;
    fd_set allSet;
    FD_ZERO(&allSet);
    FD_SET(m_nListenFd, &allSet);

    int maxfd = m_nListenFd;
    int maxi; // client max fd

    while(1)
    {
        rset = allSet;
        int nready = select(maxfd + 1, &rset, NULL, NULL, NULL); // how much ready
        if (nready < 0)
        {
            m_strErroMsg = ErrorMsg({"select()", strerror(errno)});
            clear();
            return false;
        }

        if (FD_ISSET(m_nListenFd, &rset))
        {
            sockaddr_in sockInfoCli;
            socklen_t nlen;
            int acceptFd = accept(m_nListenFd, (sockaddr*)&sockInfoCli, &nlen);
            if (acceptFd < 0)
            {
                m_strErroMsg = ErrorMsg({"accept()", strerror(errno)});
                continue;
            }
            //mkdir
            char strDir[128];
            if (inet_ntop(AF_INET, sockInfoCli.sin_addr, strDir, nlen) < 0)
            {
                m_strErroMsg = ErrorMsg({"inet_ntop()", strerror(errno)});
                continue;
            }
            mkdir(strDir, 0777); // think about it after

            std::vector<int>::iterator iter;
            iter = std::find(m_vectClients.begin(), m_vectClients.end(), -1);
            if (iter == m_vectClients.end())
                close(acceptFd); // no more descriptors send EOF
            else
            {
                *iter = acceptFd;
                FD_SET(acceptFd, &allSet); // add to allset
                std::vector<int>::const_iterator c_iter;
                c_iter = std::max_element(m_vectClients.begin(), m_vectClients.end());
                maxi = *c_iter;
                maxfd = maxi > maxfd ? maxi : maxfd;
            }
            --nready;
        }

        while(nread > 0)
        {
            char buff[MAXLINE];
            std::vector<int>::iterator iter;
            for (iter = m_vectClients.begin(); iter != m_vectClients.end(); iter++)
            {
                if (*iter == -1)
                    continue;

                if (FD_ISSET(*iter, &rset))
                {
                    int nread;
                    nread = read(*iter, buff, MAXLINE);
                    if (nread)
                    {
                        if (write(*iter, buff, nread) != nread) // first just echo cli
                        {
                            m_strErroMsg = ErrorMsg({"socket()", strerror(errno)});
                            close(*iter);
                            FD_CLR(*iter, &allSet);
                            *iter = -1;
                            std::vector<int>::const_iterator c_iter;
                            c_iter = std::max_element(m_vectClients.begin(), m_vectClients.end());
                            maxi = *c_iter;
                            maxfd = maxi > maxfd ? maxfi : maxfd;
                        }
                    }
                    if (nread == 0) // client call shutdown, nothing todo because we will send data
                    {

                    }
                    if (nread < 0) // error
                    {
                        m_strErroMsg = ErrorMsg({"read()", strerror(errno)});
                        close(*iter);
                        FD_CLR(*iter, &allSet);
                        *iter = -1;
                        std::vector<int>::const_iterator c_iter;
                        c_iter = std::max_element(m_vectClients.begin(), m_vectClients.end());
                        maxi = *c_iter;
                        maxfd = maxi > maxfd ? maxfi : maxfd;
                    }
                    --nready;
                }
            }
        }
    }
}

