set(SOURCES
    MessageQueue.cpp
    Pipe.cpp
    Process.cpp
    Semaphore.cpp
    SharedMemory.cpp
    Utility.cpp
    MemoryMap.cpp
#    
    ./exception/IpcException.cpp
    ./exception/MessageQueueException.cpp
    ./exception/NamedPipeException.cpp
    ./exception/PipeException.cpp
    ./exception/ProcessException.cpp
    ./exception/SemaphoreException.cpp
    ./exception/SharedMemoryException.cpp
    ./exception/MemoryMapException.cpp
    ./exception/FileLockException.cpp
)

set(HEADERS
    ../include/libipcpp/Definitions.h
    ../include/libipcpp/FileLock.h
    ../include/libipcpp/IOBase.h
    ../include/libipcpp/libipcpp.h
    ../include/libipcpp/LockGuard.h
    ../include/libipcpp/MemoryMap.h
    ../include/libipcpp/MessageQueue.h
    ../include/libipcpp/Pipe.h
    ../include/libipcpp/Process.h
    ../include/libipcpp/Semaphore.h
    ../include/libipcpp/SharedMemory.h
    ../include/libipcpp/Utility.h
    ../include/libipcpp/NamedPipe.h
#    
    ../include/libipcpp/exception/FileLockException.h
    ../include/libipcpp/exception/IpcException.h
    ../include/libipcpp/exception/MemoryMapException.h
    ../include/libipcpp/exception/MessageQueueException.h
    ../include/libipcpp/exception/NamedPipeException.h
    ../include/libipcpp/exception/PipeException.h
    ../include/libipcpp/exception/ProcessException.h
    ../include/libipcpp/exception/SemaphoreException.h
    ../include/libipcpp/exception/SharedMemoryException.h
)

optional_source(LINUX
    SOURCES
        platform/linux/Process.cpp
        platform/linux/Pipe.cpp
        platform/linux/Semaphore.cpp
        platform/linux/SharedMemory.cpp
        platform/linux/MemoryMap.cpp
        platform/linux/FileLock.cpp
        platform/linux/NamedPipe.cpp
        
    HEADERS
)

optional_source(WIN32
    SOURCES
        platform/win32/Process.cpp
        platform/win32/Definitions.cpp
    HEADERS
        ../include/libipcpp/platform/win32/Process.inl
)

add_library(${PROJECT_NAME_STR} SHARED ${SOURCES} ${HEADERS})