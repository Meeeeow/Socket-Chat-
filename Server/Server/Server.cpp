#include "Defines.h"


void main() {

	WSADATA		wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		cout << "Failed to Startup WSA" << endl;
		return;
	}

	SOCKET			tListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (tListen == INVALID_SOCKET)
	{
		cout << "Invalid Socket" << endl;

	}

	SOCKADDR_IN		tSocketAddress;
	ZeroMemory(&tSocketAddress, sizeof(tSocketAddress));

	tSocketAddress.sin_family = AF_INET;
	tSocketAddress.sin_port = htons(8080);
	tSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	
	int	iRet = bind(tListen, (sockaddr*)&tSocketAddress, sizeof(tSocketAddress));
	if (iRet == SOCKET_ERROR)
	{
		cout << "Failed to Binding" << endl;
		return;
	}
	
	iRet = listen(tListen, SOMAXCONN);
	if (iRet == SOCKET_ERROR)
	{
		cout << "Failed to Listening" << endl;
		return;
	}

	SOCKADDR_IN		tClientAddress = {};
	int	iSize = sizeof(tClientAddress);

	cout << "서버 가동중.." << endl;

	unsigned long On = 1;
	ioctlsocket(tListen, FIONBIO, &On);

	list<USER>		UserList;

	while (1)	{
		SOCKET	tClientSocket = accept(tListen, (sockaddr*)&tClientAddress, &iSize);
		if (tClientSocket == SOCKET_ERROR) {
			int iError = WSAGetLastError();
			if (iError != WSAEWOULDBLOCK)
			{
				cout << "ErrorCode = " << iError << endl;
				break;
			}
		}
		else {
			USER	tUser;
			tUser.Setting(tClientSocket, tClientAddress);
			UserList.push_back(tUser);

			cout << "IP  ) " << inet_ntoa(tClientAddress.sin_addr) << "\nPort  )" << ntohs(tClientAddress.sin_port) << endl;

			ioctlsocket(tClientSocket, FIONBIO, &On);
			cout << UserList.size() << " 명 접속중" << endl;
		}

		if (UserList.size() > 0) {

			auto& Begin = UserList.begin();
			auto& End = UserList.end();

			for (; Begin != End ;)
			{
				USER	tUser = *Begin;

				char	szRecvBuffer[256] = { 0, };
				int		iRecvByte = recv(tUser.m_tSocket, szRecvBuffer, 256, 0);

				if (iRecvByte == 0)	{
					closesocket(tUser.m_tSocket);
					Begin = UserList.erase(Begin);
					cout << tUser.m_strName << " 접속 종료" << endl;
					continue;
				}
				if (iRecvByte == SOCKET_ERROR) {
					int	iError = WSAGetLastError();
					if (iError != WSAEWOULDBLOCK) {
						Begin = UserList.erase(Begin);
						cout << tUser.m_strName << " 비정상 접속 종료" << endl;
					}
					else {
						Begin++;
					}
				}
				else {
					auto& SendBegin = UserList.begin();
					auto& SendEnd = UserList.end();

					for (; SendBegin != SendEnd;)
					{
						USER	tUser = *SendBegin;
						
						cout << szRecvBuffer << " 받음" << endl;

						int	iSendByte = send(tUser.m_tSocket, szRecvBuffer, iRecvByte, 0);
						cout << tUser.m_tSocket << " : " << iSendByte << " 보냄" << endl;

						if (iSendByte == SOCKET_ERROR) {
							int	iError = WSAGetLastError();
							if (iError != WSAEWOULDBLOCK) {
								closesocket(tUser.m_tSocket);
								SendBegin = UserList.erase(SendBegin);
								cout << tUser.m_strName << " 비정상 접속 종료" << endl;
							}
						}
						else {
							SendBegin++;
						}
					}
					if (SendBegin != UserList.end()) {
						SendBegin++;
					}
				}
			}
		}
	}
	closesocket(tListen);
	WSACleanup();
}