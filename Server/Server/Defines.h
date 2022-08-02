#pragma once
#ifndef __SERVER_DEFINES_H__
#define __SERVER_DEFINES_H__

#include <iostream>
#include <string>
#include <list>
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")
using namespace std;

typedef struct tagUserInfo {
	SOCKET			m_tSocket;
	SOCKADDR_IN		m_tAddress;
	string			m_strName;
	unsigned short	m_sPort;

	void Setting(SOCKET skt, SOCKADDR_IN addr) {
		m_tSocket = skt;
		m_tAddress = addr;
		m_strName = inet_ntoa(addr.sin_addr);
		m_sPort = ntohs(addr.sin_port);
	}
}USER;



#endif // !__SERVER_DEFINES_H__
