#include<iostream>
#include<string>
#include <Windows.h>
#include <Iphlpapi.h>
#include <icmpapi.h>

struct PingResult
{
	std::string host;
	int seq;
	bool ok;
	int rtt_ms;
};

void Printping(PingResult pr)
{
	std::string okText = "false";
	if (pr.ok) {
		okText = "true";
	}

	std::cout << "{\"host\":\"" << pr.host
		<< "\",\"seq\":" << pr.seq
		<< ",\"ok\":" << okText
		<< ",\"rtt_ms\":";

	if (pr.ok) {
		std::cout << pr.rtt_ms;
	} else {
		std::cout << "null";
	}

	std::cout << "}" << std::endl;
}
PingResult PingOnce(const std::string & h, int s)
{
	PingResult temp;
	temp.host = h;
	temp.seq = s;
	temp.ok = false;
	temp.rtt_ms = 0;
	HANDLE icmpHandle = IcmpCreateFile();
	if (icmpHandle == INVALID_HANDLE_VALUE) {
		return temp;
	}
	IPAddr destination = inet_addr(h.c_str());
	
	if (destination == INADDR_NONE) {
		std::cerr << "Nieprawidlowy adres IP: " << h << std::endl;
		IcmpCloseHandle(icmpHandle);
		return temp;
	}

	char replyBuffer[sizeof(ICMP_ECHO_REPLY)];
	DWORD replyCount = IcmpSendEcho(
		icmpHandle,
		destination,
		nullptr,
		0,
		nullptr,
		replyBuffer,
		sizeof(replyBuffer),
		1000
	);
	if (replyCount == 0) {
		std::cerr << "Brak odpowiedzi od "
			<< h
			<< ", kod bledu: "
			<< GetLastError()
			<< std::endl;
	}

	if (replyCount > 0)
	{
		PICMP_ECHO_REPLY reply = reinterpret_cast<PICMP_ECHO_REPLY>(replyBuffer);
		temp.ok = true;
		temp.rtt_ms = reply->RoundTripTime;
	}


	IcmpCloseHandle(icmpHandle);

	return temp;
}
int main(int argc, char* argv[])
{
	std::string host = "1.1.1.1";

	if (argc > 1) {
		host = argv[1];
	}

	for (int i = 1; i <= 5;i++)
	{
		PingResult r3 = PingOnce(host, i);
		Printping(r3);
		if (i < 5) {
			Sleep(1000);
		}
	}

	return 0;
}
