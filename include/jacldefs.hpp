#ifndef JACL_DEFS_HPP_
#define JACL_DEFS_HPP_

#include <cstdint>
#include <memory>
#include <string>

namespace jacl {

    using u8                        = std::uint8_t;
    using u32                       = std::uint32_t;
    using u64                       = std::uint64_t;

    using i8                        = std::int8_t;
    using i32                       = std::int32_t;
    using i64                       = std::int64_t;

    using f32                       = float;
    using f64                       = double;

    template <typename T> using Ref = std::shared_ptr<T>;

    constexpr u64 NOT_FOUND         = std::string::npos;

} // namespace jacl

#endif
