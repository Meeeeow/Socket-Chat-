#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <conio.h>

using namespace std;
#pragma	comment(lib, "ws2_32.lib")

void main() {

	WSADATA	wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return;
	}

	SOCKET			tSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (tSocket == INVALID_SOCKET)
	{
		cout << "Unvalid Socket" << endl;
		Sleep(1000);
	}

	SOCKADDR_IN		tSocketAddress;
	ZeroMemory(&tSocketAddress, sizeof(tSocketAddress));
	tSocketAddress.sin_family = AF_INET;
	tSocketAddress.sin_port = htons(8080);
	tSocketAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

	int	iRet = connect(tSocket, (sockaddr*)&tSocketAddress, sizeof(tSocketAddress));
	if (iRet == SOCKET_ERROR)
	{
		cout << "Failed to Connecting" << endl;
		closesocket(tSocket);
		WSACleanup();
		
		return;
	}

	cout << "Connecting!" << endl;

	unsigned long On = 1;
	ioctlsocket(tSocket, FIONBIO, &On);

	char	szBuffer[256] = { 0, };
	int		iEnd = 0;
	while (1) {
		// _kbhit = 키보드가 입력 됐는지 확인하는 함수.
		// 눌리지 않았다면 0, 눌렸다면 0 의외의 값.
		if (_kbhit() == 1) {

			int iValue = _getche();
			if (iValue == '\r' && strlen(szBuffer) == 0) {

				break;
			}
			if (iValue == '\r') {
				int iSendByte = send(tSocket, szBuffer, iEnd, 0);
				if (iSendByte == SOCKET_ERROR) {
					if (WSAGetLastError() != WSAEWOULDBLOCK) {
						cout << "비정상 서버 종료" << endl;
					}
				}
				iEnd = 0;
				ZeroMemory(szBuffer, sizeof(char) * 256);
			}
			else {
				szBuffer[iEnd++] = iValue;
			}
		}
		else {
			char	szRecvBuffer[256] = { 0, };
			int		iRecvByte = recv(tSocket, szRecvBuffer, 256, 0);

			if (iRecvByte == 0) {
				cout << "서버 종료" << endl;
				break;
			}
			if (iRecvByte == SOCKET_ERROR) {
				if (WSAGetLastError() != WSAEWOULDBLOCK) {
					cout << "비정상 서버 종료" << endl;
					break;
				}
			}
			else {
				cout << '\n' << szRecvBuffer;
				ZeroMemory(szRecvBuffer, sizeof(char) * 256);
			}
		}
	}
	cout << "접속 종료" << endl;
	closesocket(tSocket);
	WSACleanup();
	Sleep(3000);
}