/* Logging to file with different verbosity levels set using LOG_VERBOSITY 
 * log is printed in file with details like line number, date & time , vebosity level and formatted log string like below
 * 120 [ Wed May 26 13:21:00 2021 ]  [ DEBUG ] sample DEBUG
*/ 

#include <iostream>
#include <string>
#include <fstream>
#include <time.h>
#include <cstring>
#include <chrono>
#include <thread>
#include <cstdarg>

#define LOGFILE "sample_log.log"

using namespace std;

#define LOG_VERBOSITY 6

#ifndef LOG_VERBOSITY
#define LOG_VERBOSITY 2
#endif

typedef enum
{
    FATAL = 1,
    ERROR,
    WARNING,
    INFO,
    TRACE,
    DEBUG
} LOGLEVELS;

class logger
{
public:
    logger(std::string name) : filename(name)
    {
        cout << "opening " << filename << '\n';

        fout.open(filename, ios::out);

        if ((fout.rdstate() & std::fstream::failbit) != 0)
        {
            cout << "Failed to open file: " << filename << " ERROR: " << strerror(errno) << '\n';
        }
    }

    ~logger()
    {
        fout.close();
    }

    enum LOGLEVELS
    {
        FATAL = 1,
        ERROR,
        WARNING,
        INFO,
        TRACE,
        DEBUG
    };

    const char *enum_to_str(LOGLEVELS level)
    {
        switch (level)
        {
        case FATAL:
            return "FATAL";
        case ERROR:
            return "ERROR";
        case WARNING:
            return "WARNING";
        case INFO:
            return "INFO";
        case TRACE:
            return "TRACE";
        case DEBUG:
            return "DEBUG";
        default:
            return " ";
        }
    }

    void write(int line,LOGLEVELS level, const char *fmt, ...)
    {
        if (level <= LOG_VERBOSITY)
        {
            va_list args;
            va_start(args,fmt);
            char msg[1000]; 
            vsprintf(msg,fmt,args);
            fout << line <<" [ " << get_time() << " ] "
                 << " [ " << enum_to_str(level) << " ] " << msg << '\n';
        }
    }

    const char *get_time()
    {
        // time_t t = time(NULL);
        // return ctime(&t);
        auto timenow = chrono::system_clock::to_time_t(chrono::system_clock::now());
        char *time = ctime(&timenow);
        time[strcspn(time, "\r\n")] = '\0'; //to remove newline char in time
        return time;
    }

private:
    string filename;
    fstream fout;
};

static logger log(LOGFILE);

#define LOG_DEBUG(...) log.write(__LINE__,logger::LOGLEVELS::DEBUG,__VA_ARGS__)
#define LOG_FATAL(...) log.write(__LINE__,logger::LOGLEVELS::FATAL,__VA_ARGS__)
#define LOG_ERROR(...) log.write(__LINE__,logger::LOGLEVELS::ERROR,__VA_ARGS__)
#define LOG_WARNING(...) log.write(__LINE__,logger::LOGLEVELS::WARNING,__VA_ARGS__)
#define LOG_INFO(...) log.write(__LINE__,logger::LOGLEVELS::INFO,__VA_ARGS__)

int main(int argc, char* args[])
{
    LOG_DEBUG("sample DEBUG");
    LOG_FATAL("sample FATAL");
    LOG_WARNING("sample WARNING");
    LOG_INFO("sample INFO");
    LOG_DEBUG("sample DEBUG %d %s",100,"formatted string");
    return 0;
}
