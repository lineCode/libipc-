#ifdef __linux__
#include "../../../include/libipcpp/Utility.h"
#include "../../../include/libipcpp/Process.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <signal.h>
#include <algorithm>
#include <fstream>
#include <vector>

namespace ipc {
    
   
    Process::Process(const std::string& fileName, const std::vector<std::string>& args) throw(ProcessException)
    {
        std::string str;
        if(!args.empty()){
            for(const std::string& arg : args) {
                if(str.empty())
                    str += arg;
                else
                    str += " " + arg;
            }
        }
        
        pid_t pid = fork();
        std::vector<char *> argV;
        
        switch(pid) {
            case -1:
                throw ProcessException("Process could not be created.");
                break;
            case 0:
                argV.push_back(const_cast<char *>(fileName.c_str()));
                argV.push_back(const_cast<char *>(str.c_str()));
                argV.push_back(nullptr);
                execv(argV[0], &argV[0]);
                break;
            default:
                mProcessInfo.mHandle = pid;
                mIsOwner = true;
        }
    }
    
    Process::Process(const std::string& fileName, const std::string& args) throw (ipc::ProcessException) 
    {
        pid_t pid = fork();
        std::vector<char *> argV;
        
        switch(pid) {
            case -1:
                throw ProcessException("Process could not be created.");
                break;
            case 0:
                argV.push_back(const_cast<char *>(fileName.c_str()));
                argV.push_back(const_cast<char *>(args.c_str()));
                argV.push_back(nullptr);
                execv(argV[0], &argV[0]);
                break;
            default:
                mProcessInfo.mHandle = pid;
                mIsOwner = true;
        }
    }
    
    Process::Process(const ProcessInfo& info)
    {
        mProcessInfo = info;
        if (!IsValid()) {
            throw ProcessException("Process handle is not valid.");
        }
    }

    Process::Process(Process&& p) :
        mIsOwner(p.mIsOwner), mProcessInfo(p.mProcessInfo)
    {
        p.mIsOwner = false;
        p.mProcessInfo.mHandle = PROCESS_INVALID_HANDLE;
    }

    Process::~Process()
    {
        if(!mIsOwner){
            return;
        }
            
        try {
            Kill();
            
        } catch (ProcessException) {
            
        }
    }

    int32_t Process::ExitCode() const throw(ProcessException)
    {
        if(WIFEXITED(mStatus))
            return WEXITSTATUS(mStatus);
        
        throw ProcessException("Process is still running.");
    }

    void Process::Kill() throw(ProcessException)
    {
        if(mProcessInfo.mHandle == PROCESS_INVALID_HANDLE)
            return;
        
        int rv = kill(mProcessInfo.mHandle, SIGKILL);
        if(rv < 0)
            throw ProcessException("Process could not be killed.");
        waitpid(mProcessInfo.mHandle, &mStatus, 0);
    }

    Process& Process::Wait() throw(ProcessException)
    {
        waitpid(mProcessInfo.mHandle, &mStatus, 0);
        return *this;
    }

    std::vector<ProcessInfo> Process::GetProcessByName(const std::string& name) throw(ProcessException)
    {
        DIR *directory = opendir("/proc");
        std::vector<ProcessInfo> results;
        
        if(directory) {
            struct dirent *dirEntry;
            while ((dirEntry = readdir(directory)))
            {
                if(!IsNumber(dirEntry->d_name))
                    continue;

                int nextPid = std::stoi(dirEntry->d_name);

                std::string path = std::string("/proc/") + dirEntry->d_name + "/cmdline";
                std::ifstream file(path);
                std::string line;
                std::getline(file, line, '\0');
                
                if(line.empty())
                    continue;
                std::vector< std::string > tokens;
                SplitString(line, "/", tokens);

                std::string processCandidate = tokens.back();
                auto it = std::search(processCandidate.begin(), processCandidate.end(), name.begin(), name.end());
                if(it != processCandidate.end()) {
                    ProcessInfo tmp; 
                    tmp.mId = nextPid;
                    tmp.mHandle = nextPid;
                    tmp.mName = name;
                    results.push_back(tmp);
                }
                    
            }
        }
        return results;
    }

    std::vector<ProcessInfo> Process::GetProcesses() throw(ProcessException)
    {
        
        DIR *directory = opendir("/proc");
        std::vector<ProcessInfo> results;
        
        if(directory) {
            struct dirent *dirEntry;
            while ((dirEntry = readdir(directory)))
            {
                if(!IsNumber(dirEntry->d_name))
                    continue;

                int nextPid = std::stoi(dirEntry->d_name);

                std::string path = std::string("/proc/") + dirEntry->d_name + "/cmdline";
                std::ifstream file(path);
                std::string line;
                std::getline(file, line, '\0');
                
                ProcessInfo tmp;
                tmp.mId = nextPid;
                tmp.mHandle = nextPid;
                tmp.mName = line;
                results.push_back(tmp);
            }
        }
        
        return results;
    }
    
    bool Process::IsValid() NOEXCEPT
    {
        ValidateProcessHandle();
        return (mProcessInfo.mHandle != PROCESS_INVALID_HANDLE);
    }
    
    
    void Process::ValidateProcessHandle() 
    {
        int rv = kill(mProcessInfo.mHandle, 0);
        if (rv == 0) {
            mIsOwner = true;
        }
        else if(rv == EPERM) {
            mIsOwner = false;
        } else if(rv < 0) {
            mProcessInfo.mHandle = PROCESS_INVALID_HANDLE;
        }
    }
    


}
#endif