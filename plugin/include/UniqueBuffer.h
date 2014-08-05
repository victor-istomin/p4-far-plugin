#pragma once
#include "NonCopyable.h"
#include <type_traits>
#include <utility>

#ifndef NDEBUG
#  define DEBUG_MEMBER  CastTo m_debugPtr;
#  define DEBUG_PTR     , m_debugPtr((CastTo)m_buffer)
#  define DEBUG_PTR_UPD m_debugPtr = (CastTo)m_buffer;
#else
#  define DEBUG_MEMBER  
#  define DEBUG_PTR
#  define DEBUG_PTR_UPD
#endif

template <typename CastTo = char* const>
class UniqueBuffer 
    : public NonCopyable
{
    typedef typename std::is_pointer<CastTo> TCheckPointer;

    static_assert(TCheckPointer::value == true, "<CastTo> should be a pointer");

    char*  m_buffer;
    size_t m_size;

    DEBUG_MEMBER;

    UniqueBuffer<CastTo>& operator=(const UniqueBuffer<CastTo>&); // = delete;

    void clear()
    {
        delete [] m_buffer;
        m_buffer = nullptr;
        m_size   = 0;
        DEBUG_PTR_UPD;
    }

public:

    void reset(size_t size = 0)
    {
        std::swap(*this, UniqueBuffer<CastTo>(size));
    }

    UniqueBuffer(size_t size = 0)
        : m_buffer(size ? new char[size] : nullptr)
        , m_size(size)
        DEBUG_PTR
    {
    }

    UniqueBuffer(UniqueBuffer&& moveFrom)
        : m_buffer(nullptr)
        , m_size(0)
        DEBUG_PTR
    {
        std::swap(m_buffer, moveFrom.m_buffer);
        std::swap(m_size,   moveFrom.m_size);
        DEBUG_PTR_UPD;
    }

    UniqueBuffer<CastTo>& operator=(UniqueBuffer<CastTo>&& moveFrom)
    {
        clear();

        std::swap(m_buffer, moveFrom.m_buffer);
        std::swap(m_size,   moveFrom.m_size);
        DEBUG_PTR_UPD;

        return *this;
    }

    ~UniqueBuffer()
    {
        clear();
    }

    size_t       size() const      { return m_size; }
    const CastTo get() const        { return reinterpret_cast<CastTo>(m_buffer); }
    CastTo       get()              { return reinterpret_cast<CastTo>(m_buffer); }
    const CastTo operator->() const { return get(); }
    CastTo       operator->()       { return get(); }

    const char* begin() const       { return m_buffer; }
    char*       begin()             { return m_buffer; }
    const char* end() const         { return begin() + size(); }
    char*       end()               { return begin() + size(); }
};


