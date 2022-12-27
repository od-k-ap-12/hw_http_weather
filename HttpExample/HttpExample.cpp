#pragma comment (lib, "Ws2_32.lib")
#include <Winsock2.h>
#include <ws2tcpip.h>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

string JSONFind(string& Text, string& Find);

int main()
{
    setlocale(0, "ru");
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 2);

    int err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {

        cout << "WSAStartup failed with error: " << err << endl;
        return 1;
    }
    char hostname[255] = "api.openweathermap.org";

    addrinfo* result = NULL;

    addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int iResult = getaddrinfo(hostname, "http", &hints, &result);
    if (iResult != 0) {
        cout << "getaddrinfo failed with error: " << iResult << endl;
        WSACleanup();
        return 3;
    }

    SOCKET connectSocket = INVALID_SOCKET;
    addrinfo* ptr = NULL;

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (connectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(connectSocket);
            connectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    string City, CityID = "\"id\"", Country = "\"country\"", TempMin = "\"temp_min\"", TempMax = "\"temp_max\"", Sunrise = "\"sunrise\"", Sunset = "\"sunset\"", lon = "\"lon\"", lat = "\"lat\"", WeatherInfo;
    cout << "Enter city: " << endl;
    cin >> City;
    string uri = "/data/2.5/weather?q=" + City + "&units=metric&appid=75f6e64d49db78658d09cb5ab201e483&mode=JSON";

    string request = "GET " + uri + " HTTP/1.1\n";
    request += "Host: " + string(hostname) + "\n";
    request += "Accept: */*\n";
    request += "Accept-Encoding: gzip, deflate, br\n";
    request += "Connection: close\n";
    request += "\n";

    if (send(connectSocket, request.c_str(), request.length(), 0) == SOCKET_ERROR) {
        cout << "send failed: " << WSAGetLastError() << endl;
        closesocket(connectSocket);
        WSACleanup();
        return 5;
    }
    cout << "send data" << endl;


    string response;

    const size_t BUFFERSIZE = 1024;
    char resBuf[BUFFERSIZE];

    int respLength;

    do {
        respLength = recv(connectSocket, resBuf, BUFFERSIZE, 0);
        if (respLength > 0) {
            response += string(resBuf).substr(0, respLength);
        }
        else {
            cout << "recv failed: " << WSAGetLastError() << endl;
            closesocket(connectSocket);
            WSACleanup();
            return 6;
        }

    } while (respLength == BUFFERSIZE);

    cout << "---------------------------------------------------------------------" << endl << endl;
    WeatherInfo = WeatherInfo + "City:" + City + '\n';
    WeatherInfo = WeatherInfo + CityID + ":" + JSONFind(response, CityID) + '\n';
    WeatherInfo = WeatherInfo + Country + ":" + JSONFind(response, Country) + '\n';
    WeatherInfo = WeatherInfo + TempMin + ":" + JSONFind(response, TempMin) + '\n';
    WeatherInfo = WeatherInfo + TempMax + ":" + JSONFind(response, TempMax) + '\n';
    WeatherInfo = WeatherInfo + Sunrise + ":" + JSONFind(response, Sunrise) + '\n';
    WeatherInfo = WeatherInfo + Sunset + ":" + JSONFind(response, Sunset) + '\n';
    WeatherInfo = WeatherInfo + lon + ":" + JSONFind(response, lon) + '\n';
    WeatherInfo = WeatherInfo + lat + ":" + JSONFind(response, lat) + '\n';
    cout << WeatherInfo;
    ofstream File("Weather.txt", ios::app);
    if (File.is_open()) {
        File << WeatherInfo;
    }
    File.close();
    cout << endl << "File saved";


    //отключает отправку и получение сообщений сокетом
    iResult = shutdown(connectSocket, SD_BOTH);
    if (iResult == SOCKET_ERROR) {
        cout << "shutdown failed: " << WSAGetLastError() << endl;
        closesocket(connectSocket);
        WSACleanup();
        return 7;
    }

    closesocket(connectSocket);
    WSACleanup();
}

string JSONFind(string& Text, string& Find) {
    string Data;
    for (auto i = Text.begin() + Text.find(Find) + Find.length() + 1; *i != ',' && *i != '}'; i++) {
        Data += *i;
    }
    return Data;
}