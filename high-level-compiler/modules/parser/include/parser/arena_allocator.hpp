#pragma once
#include <cstddef>
#include <cstdlib>

namespace parser {

class ArenaAllocator{
public:
    explicit ArenaAllocator(size_t arena_size);
    ~ArenaAllocator();

    ArenaAllocator(ArenaAllocator& o) = delete;
    ArenaAllocator(ArenaAllocator&& o) = delete;
    ArenaAllocator& operator=(ArenaAllocator& o) = delete;
    ArenaAllocator& operator=(ArenaAllocator&& o) = delete;

    template <typename T>
    T* alloc() {
        if (m_offset+sizeof(T) >= m_size) {
            std::byte* buffer = static_cast<std::byte*>(std::realloc(m_buffer, m_size*2)); 
            m_size *= 2;
            m_buffer = buffer;
        }
        void* block = m_buffer+m_offset;     
        m_offset += sizeof(T);
        return static_cast<T*>(block);
    }

private:
    size_t m_size;
    size_t m_offset;
    std::byte* m_buffer;
};

} // namespace parser
