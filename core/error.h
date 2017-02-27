#pragma once

namespace Core
{

class Error
{
public:
    Error();

    Error(int code);

    virtual ~Error();

    int GetCode() const;

    void SetCode(int code);

    const char* GetDescription() const;

    static const int Success = 0;

    static const int Again = -11;

    static const int InvalidValue = -22;

    static const int NoMemory = -12;

    static const int ConnReset = -104;

    static const int Cancelled = -125;

    static const int NotExecuted = -500;

    static const int InvalidState = -501;

    static const int Unsuccessful = -502;

    static const int NotImplemented = -503;

    static const int UnknownCode = -504;

    static const int NotFound = -505;

    static const int EOF = -506;

    static const int BufToBig = -507;

    static const int IO = -508;

    static const int BadMagic = -509;

    static const int HeaderCorrupt = -510;

    static const int DataCorrupt = -511;

    static const int BadSize = -512;

    static const int AlreadyExists = -513;

    static const int UnexpectedEOF = -514;

    static const int Overflow = -515;

    static const int Overlap = -515;

    bool operator!= (const Error& other) const;

    bool operator== (const Error& other) const;

    bool Ok() const
    {
        return (Code == Success) ? true : false;
    }

    void SetNoMemory()
    {
        Code = NoMemory;
    }

    void SetEOF()
    {
        Code = EOF;
    }

    void SetInvalidState()
    {
        Code = InvalidState;
    }

    void SetInvalidValue()
    {
        Code = InvalidValue;
    }

    void SetNotImplemented()
    {
        Code = NotImplemented;
    }

    void Clear()
    {
        Code = Success;
    }

private:
    int Code;
};

}