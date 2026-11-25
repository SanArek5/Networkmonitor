#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#ifdef _MSC_VER
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#endif

struct PingResult
{
    std::string host;
    int seq;
    bool ok;
    int rtt_ms;
}; //struktura pinga 

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
    }
    else {
        std::cout << "null";
    }

    std::cout << "}" << std::endl;
}// funkcja do konsolowego wyswietlania ping i jego rezulatatu

PingResult PingOnce(const std::string& h, int s)
{
    PingResult temp;

    temp.host = h;
    temp.seq = s;
    temp.ok = false;
    temp.rtt_ms = 0;

    HANDLE icmpHandle = IcmpCreateFile(); // otwarcie obsugi pingow IMCP

    if (icmpHandle == INVALID_HANDLE_VALUE) {
        return temp; 
    } 

    addrinfo* result = nullptr; //zmienna wyniku wyszukiwania dnsem
    addrinfo hints{}; //ustawienia wyszukiwania adresu

    hints.ai_family = AF_INET;

    int dnsResult = getaddrinfo(
        h.c_str(),
        nullptr,
        &hints,
        &result
    ); // zamiana nazwy hosta na ipv4

    if (dnsResult != 0 || result == nullptr) {
        std::cerr << "Nie znaleziono hosta: "
            << h
            << ", kod bledu: "
            << dnsResult
            << std::endl;

        IcmpCloseHandle(icmpHandle);
        return temp; // koniec pomiaru jezeli nie znaleziono hosta dnsem
    }

    sockaddr_in* ipv4Address = reinterpret_cast<sockaddr_in*>(result->ai_addr);
    // Pobieranie znalezionego adresu IPv4.
    
    IPAddr destination = ipv4Address->sin_addr.s_addr;
    // Zapis adresu w formacie wymaganym przez IcmpSendEcho.

    freeaddrinfo(result); //zwolnienie pamieci uzywanej przez wynik dns

    char replyBuffer[sizeof(ICMP_ECHO_REPLY) + 32];


    DWORD replyCount = IcmpSendEcho(
        icmpHandle,
        destination,
        nullptr,
        0,
        nullptr,
        replyBuffer,
        sizeof(replyBuffer),
        1000
    );//wysylanie pingu na max 1000ms

    if (replyCount == 0) {
        std::cerr << "Brak odpowiedzi od "
            << h
            << ", kod bledu: "
            << GetLastError()
            << std::endl;
    }
    else {
        PICMP_ECHO_REPLY reply = reinterpret_cast<PICMP_ECHO_REPLY>(replyBuffer);
        // odczyt odpowiedzi z bufforu 

        temp.ok = true;
        temp.rtt_ms = reply->RoundTripTime;
        // zapisanie pomiaru i czasu odpowiedzi
    }

    IcmpCloseHandle(icmpHandle);

    return temp;
}

int main(int argc, char* argv[])
{
    WSADATA wsaData; 
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData); //uruchamia systemową bibliotekę sieciową Windows
    if (wsaResult != 0) {
        std::cerr << "Nie udalo sie uruchomic Winsock" << std::endl;
        return 1;
    }

    std::string host = "1.1.1.1"; //domyslny host

    if (argc > 1) {
        host = argv[1];
    }

    for (int i = 1; i <= 5; i++)
    {
        PingResult r3 = PingOnce(host, i);
        Printping(r3);
        if (i < 5) {
            Sleep(1000);
        }
    }

    WSACleanup(); 
    return 0;
}