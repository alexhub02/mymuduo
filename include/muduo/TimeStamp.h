#ifndef MYMUDUO_TIMESTAMP_H
#define MYMUDUO_TIMESTAMP_H
#include <iostream>
#include <string>

class TimeStamp
{
public:
    TimeStamp();
    explicit TimeStamp(int64_t microSecondsSinceEpoch);
    static TimeStamp now();
    std::string toString() const;
private:
    int64_t microSecondsSinceEpoch_;
};
#endif
