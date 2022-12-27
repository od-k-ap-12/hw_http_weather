#include <iostream>
#include <windows.h>
#pragma comment(lib, "urlmon.lib")
using namespace std;

int main()
{
    // the URL to download from
    const char* srcURL = "https://www.gismeteo.ua/weather-odessa-4982/";

    // the destination file
    const char* destFile = "weather.txt";

    // URLDownloadToFile returns S_OK on success
    if (S_OK == URLDownloadToFileA(NULL, srcURL, destFile, 0, NULL))
    {
        cout << "Saved to " << destFile << "\n";
    }
   
}