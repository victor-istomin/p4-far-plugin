#pragma once
#ifndef _NON_COPYABLE_H_
#define _NON_COPYABLE_H_


class NonCopyable
{
public:
    NonCopyable() {}
private:
    NonCopyable(const NonCopyable&); //!< denied
    NonCopyable& operator=(const NonCopyable&); //!< denied
};

#endif