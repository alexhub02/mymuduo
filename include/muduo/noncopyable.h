#ifndef MYMUDUO_NONCOPYABLE_H
#define MYMUDUO_NONCOPYABLE_H
class noncopyable
{
public:
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
protected:
    noncopyable() = default;
    ~noncopyable() = default;
};
#endif //MYMUDUO_NONCOPYABLE_H
