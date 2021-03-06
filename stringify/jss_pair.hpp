#pragma once

#include "jss_object.hpp"
#include "jss_optional.hpp"
#include <utility>

namespace JSON
{
    template <typename KeyT, typename ValueT,
              typename = typename std::enable_if <Internal::can_stringify<KeyT>::value && Internal::can_stringify<ValueT>::value>::type >
    std::ostream& stringify(std::ostream& stream, std::string const& name, std::pair <KeyT, ValueT> const& value, StringificationOptions options)
    {
        SJSON_WRITE_OBJECT_START(stream);
        options.ignore_name = false;
        options.in_object = true;
        stringify(stream, "first", value.first, options);
        if (Internal::is_optional_set(value.first))
            stream << options.delimiter;
        stringify(stream, "second", value.second, options);
        SJSON_WRITE_OBJECT_END(stream);
        return stream;
    }
}
