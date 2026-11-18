#include "parser/arena_allocator.hpp"


namespace parser {

ArenaAllocator::ArenaAllocator(size_t arena_size)
    : m_size{arena_size} 
    , m_offset{0}
{
    m_buffer = static_cast<std::byte*>(std::malloc(arena_size));
}

ArenaAllocator::~ArenaAllocator() {
    free(m_buffer);
}

} // namespace parser
