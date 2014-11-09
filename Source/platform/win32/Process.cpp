#ifdef _MSC_VER
#include "../../../include/libipcpp/Process.h"
#include <Psapi.h>

using string = std::string;
template <typename T>
using vector = std::vector < T > ;
template <typename T>
using shared_ptr = std::shared_ptr < T > ;
template <typename T>
using make_shared = std::make_shared < T > ;

namespace ipc {
    ProcessInfo::~ProcessInfo()
    {
        if (mHandle != PROCESS_INVALID_HANDLE) {
            CloseHandle(mHandle);
            mHandle = PROCESS_INVALID_HANDLE;
        }
    }

    Process::Process(const string& fileName, const vector<string>& args)
    {
        string str = fileName;

        for (const string& arg : args) {
            str += " " + arg;
        }

        Process::Process(fileName, args);
    }

    Process::Process(const string& fileName, const string& args)
    {
        LPSTR cmdl = new CHAR[args.size() + 1];
        STARTUPINFO startupInfo;
        PROCESS_INFORMATION processInformation;
        memset(&startupInfo, 0, sizeof(startupInfo));
        memset(cmdl, 0, args.size() + 1);
        memcpy(cmdl, args.data(), args.size());

        startupInfo.cb = sizeof(startupInfo);

        BOOL result = CreateProcessA(fileName.c_str(), cmdl, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInformation);
        delete cmdl;

        if (!result) {
            // TODO: Systeminformation abrufen und als Argument �bergeben.
            throw ProcessException("");
        } else {
            mIsOwner = true;
            mProcess = processInformation.hProcess; // TODO: hThread speichern.
            mState = ProcessState::IsRunning;
        }
    }

    Process::Process(Process&& p) :
        mIsOwner(p.mIsOwner), mProcess(p.mProcess), mState(p.mState)
    {
        p.mIsOwner = false;
        p.mProcess = PROCESS_INVALID_HANDLE;
        p.mState = ProcessState::Invalid;
    }

    Process::~Process()
    {
        if (mIsOwner) {
            if (mProcess != PROCESS_INVALID_HANDLE) {
                CloseHandle(mProcess);
                mProcess = PROCESS_INVALID_HANDLE;
            }

            // TODO: mThread schlie�en.
        }
    }

    int32_t Process::ExitCode() const
    {
        DWORD exitCode = ~0;
        
        if (!GetExitCodeProcess(mProcess, &exitCode)) {
            // TODO: Systeminformation abrufen und als Argument �bergeben.
            throw ProcessException("");
        } else {
            return static_cast<int32_t>(exitCode);
        }
    }

    void Process::Kill()
    {
        // TODO: Bestimmter ExitCode?
        BOOL result = TerminateProcess(mProcess, ~0);

        if (!result) {
            // TODO: Systeminformation abrufen und als Argument �bergeben.
            throw ProcessException("");
        } else {
            mProcess = PROCESS_INVALID_HANDLE;
            mState = ProcessState::Invalid;
        }

        // TODO: mThread terminieren.
    }

    Process& Process::Wait()
    {
        if (WaitForSingleObject(mProcess, INFINITE) == WAIT_FAILED) {
            // TODO: Systeminformation abrufen und als Argument �bergeben.
            throw ProcessException("");
        } else {
            mProcess = PROCESS_INVALID_HANDLE;
            mState = ProcessState::Invalid;
        }
    }

    vector<shared_ptr<Process>> Process::GetProcessByName(const string& name)
    {
        vector<shared_ptr<Process>> result = nullptr;
        DWORD aProcesses[1024];
        DWORD cbNeeded;
        DWORD cProcesses;

        if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
            // TODO: Systeminformation abrufen und als Argument �bergeben.
            throw ProcessException("");
        }

        cProcesses = cbNeeded / sizeof(DWORD);

        for (DWORD i = 0; i < cProcesses; i++) {
            if (aProcesses[i] != 0) {
                CHAR szProcessName[MAX_PATH] = "<unknown>";
                HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);

                if (hProcess) {
                    HMODULE hMod;
                    DWORD cbNeeded;

                    if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
                        GetModuleBaseNameA(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(CHAR));
                        
                        if (szProcessName == name.c_str()) {
                            result.push_back(make_shared<Process>(hProcess));
                        }
                    }
                }
            }
        }

        return result;
    }

    vector<ProcessInfo> Process::GetProcesses()
    {
        vector<ProcessInfo> result;
        DWORD aProcesses[1024];
        DWORD cbNeeded;
        DWORD cProcesses;

        if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
            // TODO: Systeminformation abrufen und als Argument �bergeben.
            throw ProcessException("");
        }

        cProcesses = cbNeeded / sizeof(DWORD);

        for (DWORD i = 0; i < cProcesses; i++) {
            if (aProcesses[i] != 0) {
                CHAR szProcessName[MAX_PATH] = "<unknown>";
                HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);

                if (hProcess) {
                    HMODULE hMod;
                    DWORD cbNeeded;

                    if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
                        GetModuleBaseNameA(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(CHAR));
                        ProcessInfo info;
                        info.mHandle = hProcess;
                        info.mId = static_cast<int64_t>(aProcesses[i]);
                        info.mName = szProcessName;
                        result.push_back(info);
                    }
                }
            }
        }

        return result;
    }
}
#endif
