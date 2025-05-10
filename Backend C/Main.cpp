#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <tlhelp32.h>

#define FILE_PATH "C:\\sysmon_data.txt"
#define UI_PROCESS_NAME "SystemMonitor.exe"


// Function to check if the UI process is running
int is_ui_running() {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (Process32First(snapshot, &entry)) {
        do {
            if (strcmp(entry.szExeFile, UI_PROCESS_NAME) == 0) {
                CloseHandle(snapshot);
                return 1;  // UI is running
            }
        } while (Process32Next(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return 0;  // UI is not running
}

//
void get_time_and_temp() {
    FILE *fp = fopen(FILE_PATH, "w");  // overwrite to file
    if (!fp) return;

    // Get current time..........................................
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char dateTimeStr[30];
    strftime(dateTimeStr, sizeof(dateTimeStr), "%d-%m-%y %H:%M", t);

    // Get CPU temperature using WMIC..........................................................
    char tempStr[10] = "N/A";
    FILE *tempFile = _popen("wmic /namespace:\\\\root\\wmi PATH MSAcpi_ThermalZoneTemperature get CurrentTemperature 2>nul", "r");
    
    if (tempFile) {
        char buffer[128];
        fgets(buffer, sizeof(buffer), tempFile); // Skip first line
        if (fgets(buffer, sizeof(buffer), tempFile)) {
            int temp;
            if (sscanf(buffer, "%d", &temp) == 1) {
                sprintf(tempStr, "%.1f", (temp / 10.0) - 273.15); // Convert Kelvin to Celsius
            }
        }
        _pclose(tempFile);
    }

    // Get CPU Usage.......................................................................
    FILE *cpuFile = _popen("wmic cpu get loadpercentage 2>nul", "r");
    char cpuUsageStr[10] = "N/A";
    if (cpuFile) {
        char buffer[128];
        fgets(buffer, sizeof(buffer), cpuFile); // Skip first line
        if (fgets(buffer, sizeof(buffer), cpuFile)) {
            sscanf(buffer, "%s", cpuUsageStr);
        }
        _pclose(cpuFile);
    }

    // Get Live CPU Speed using PowerShell...................................................
    char handleCountStr[20] = "N/A";  
    FILE *handleFile = _popen("powershell -WindowStyle Hidden -command \"(Get-Process | Measure-Object Handles -Sum).Sum\"", "r");

    if (handleFile) {
        if (fgets(handleCountStr, sizeof(handleCountStr), handleFile)) {
            int handleCount;
            if (sscanf(handleCountStr, "%d", &handleCount) == 1) {
                sprintf(handleCountStr, "%d", handleCount); // Store the handle count
            }
        }
        _pclose(handleFile);
    }


     // Get Number of Threads..........................................................
     char threadCountStr[10] = "N/A";
     FILE *threadFile = _popen("powershell -WindowStyle Hidden -command \"(Get-WmiObject Win32_PerfRawData_PerfOS_System).Threads\"", "r");
     
     if (threadFile) {
         if (fgets(threadCountStr, sizeof(threadCountStr), threadFile)) { // Read first (and only) line
             int threadCount;
             if (sscanf(threadCountStr, "%d", &threadCount) == 1) {
                 sprintf(threadCountStr, "%d", threadCount); // Store the thread count
             }
         }
         _pclose(threadFile);
     }
 
        // Get Number of Running Processes.....................................................
        char processCountStr[10] = "N/A";
        FILE *processFile = _popen("powershell -command \"(Get-Process | Measure-Object).Count\"", "r");
        if (processFile) {
            char buffer[128];
            if (fgets(buffer, sizeof(buffer), processFile)) {
                sscanf(buffer, "%s", processCountStr);
            }
            _pclose(processFile);
        }


    // Get Memory Usage................................................................
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    DWORDLONG totalMemory = memInfo.ullTotalPhys / (1024 * 1024);
    DWORDLONG usedMemory = (memInfo.ullTotalPhys - memInfo.ullAvailPhys) / (1024 * 1024);

    // Get Disk Space.................................................................
    ULARGE_INTEGER freeBytes, totalBytes, freeBytesAvailable;
    GetDiskFreeSpaceEx("C:\\\\", &freeBytesAvailable, &totalBytes, &freeBytes);
    if (!GetDiskFreeSpaceEx("C:\\\\", &freeBytesAvailable, &totalBytes, &freeBytes)) {
        printf("Error getting disk space: %lu\n", GetLastError());
        return;
    }
    DWORDLONG totalDisk = totalBytes.QuadPart / (1024 * 1024 * 1024);
    DWORDLONG freeDisk = freeBytes.QuadPart / (1024 * 1024 * 1024);

    
    // Write Data to File..................................................
    fprintf(fp, "%s\t%s\t%s\t%s\t%s\t%s\t%llu\t%llu\t%llu\t%llu\n", dateTimeStr, tempStr, cpuUsageStr,processCountStr,threadCountStr,handleCountStr, totalMemory, usedMemory, totalDisk, freeDisk);
    fflush(fp);
    fclose(fp);
}

int main() {
    
    system("start SystemMonitor.exe"); // Start Qt UI

     // Keep running while the UI is active
     while (is_ui_running()) {
        get_time_and_temp();
        Sleep(200);  // Update every second
    }

    return 0;
}