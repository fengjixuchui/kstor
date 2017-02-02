#pragma once

namespace KStor
{

namespace Control
{

class Ctl
{
public:
    Ctl(int& err);

    int GetTime(unsigned long long& time);
    int GetRandomUlong(unsigned long& value);

    int Mount(const char* deviceName, bool format, unsigned long& deviceId);
    int Unmount(unsigned long& deviceId);
    int Unmount(const char* deviceName);

    int StartServer(const char *host, unsigned short port);
    int StopServer();

    virtual ~Ctl();
private:
    int DevFd;
};

}

}