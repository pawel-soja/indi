#pragma once
/*
    Copyright (C) 2020 by Pawel Soja <kernel32.pl@gmail.com>
    UniqueBuffer

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/
#include <memory>
#include <cstring>
#include <cstdint>

namespace INDI
{

/**
 * \class UniqueBuffer
 * \brief Class to provide storing frame pointer (and size) like unique_ptr in uint8_t * format.
 *        Additionally, it has useful functions for copying data.
 */
class UniqueBuffer
{
    uint8_t *m_data;
    size_t   m_size;

    UniqueBuffer(const UniqueBuffer &) = delete;
    UniqueBuffer& operator=(UniqueBuffer const&) = delete;

public:
    UniqueBuffer();
    UniqueBuffer(uint8_t *data, size_t size);
    UniqueBuffer(UniqueBuffer && other);
    ~UniqueBuffer();

    UniqueBuffer &operator=(UniqueBuffer && other);

public:
    /**
     *  @brief Create copy as UniqueBuffer
     *  @param data source buffer to be copyied
     *  @param size size in bytes of data to be copied
     *  @return UniqueBuffer as copy of source data
     */
    static UniqueBuffer copy(const uint8_t *data, size_t size);

public:
    /**
     * @brief Returns the size of the buffer in bytes
     */
    size_t size() const;

    /**
     * @brief Returns a buffer pointer
     */
    uint8_t *data() const;

    /**
     * @brief Returns a buffer pointer and release the buffer from the class.
     * You have to take care of the free buffer later.
     */
    uint8_t *release();
};




// inline implementations

inline UniqueBuffer::UniqueBuffer()
    : m_data(nullptr), m_size(0)
{ }

inline UniqueBuffer::UniqueBuffer(uint8_t *data, size_t size)
    : m_data(data), m_size(size)
{ }

inline UniqueBuffer::~UniqueBuffer()
{ if (m_data) free(m_data); }

inline UniqueBuffer::UniqueBuffer(UniqueBuffer && other)
    : m_data(other.m_data), m_size(other.m_size)
{ other.m_data = nullptr; other.m_size = 0; }

inline size_t UniqueBuffer::size() const
{ return m_size; }

inline uint8_t *UniqueBuffer::data() const
{ return m_data; }


inline UniqueBuffer UniqueBuffer::copy(const uint8_t *data, size_t size)
{
    uint8_t* copy = static_cast<uint8_t*>(malloc(size));

    if(copy != nullptr)
        ::memcpy(copy, data, size);

    return UniqueBuffer(copy, size);
}

inline uint8_t *UniqueBuffer::release()
{
    uint8_t *result = m_data;
    m_data = nullptr;
    m_size = 0;
    return result;
}

inline UniqueBuffer &UniqueBuffer::operator=(UniqueBuffer && other)
{
    std::swap(other.m_data, m_data);
    std::swap(other.m_size, m_size);
    return *this;
}

}
