#ifndef BASE64_H_INCLUDED
#define BASE64_H_INCLUDED

#include "../stringify/jss_object.h"
#include "../parse/jsd_core.h"

#include <string>
#include <iostream>
#include <stdexcept>

/**
 *  Provides a wrapper called Base64Binary for use in classes instead of std::vector <char> etc.
 *  It is meant to compress binary data down.
 *
 *  And it is only meant for that, it does not produce correct output if shoved into stringify itself.
 */

namespace JSON
{
    template <template <typename T, typename = std::allocator <T> > class ContainerT>
    void encodeBase64(std::ostream& stream, ContainerT <char> const& bytes);
    template <template <typename T, typename = std::allocator <T> > class ContainerT>
    void decodeBase64(std::string const& input, ContainerT <char>& bytes);

    template <template <typename T, typename = std::allocator <T> > class ContainerT>
    struct Base64Binary
    {
    private:
        ContainerT <char> binary_;

    public:
        ContainerT <char>& get() { return binary_; }

        explicit Base64Binary (ContainerT <char> container)
            : binary_(std::move(container))
        { }

        Base64Binary& operator= (ContainerT <char> const& container)
        {
            binary_ = container;
            return this;
        }

        operator ContainerT <char>& () { return binary_; }

        typename ContainerT <char>::iterator begin() { return binary_.begin(); }
        typename ContainerT <char>::iterator end() { return binary_.end(); }
        typename ContainerT <char>::const_iterator cbegin() const { return binary_.cbegin(); }
        typename ContainerT <char>::const_iterator cend() const { return binary_.cend(); }
        typename ContainerT <char>::const_iterator begin() const { return binary_.begin(); }
        typename ContainerT <char>::const_iterator end() const { return binary_.end(); }

        std::ostream& stringify(std::ostream& stream, StringificationOptions const& options = DEFAULT_OPTIONS) const
        {
            stream << "\"";
            encodeBase64(stream, binary_);
            stream << "\"";
            return stream;
        }

        void parse(std::string const& name, PropertyTree const& tree, ParsingOptions const& options = DEFAULT_PARSER_OPTIONS)
        {
            std::string str = tree.tree.get<std::string>(name);
            decodeBase64(str, binary_);
        }
    };
}

namespace JSON
{
    static char const table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

    template <template <typename T, typename = std::allocator <T> > class ContainerT>
    void encodeBase64(std::ostream& stream, ContainerT <char> const& bytes)
    {
        std::size_t b;
        for (std::size_t i = 0; i < bytes.size(); i += 3)
        {
            b = (bytes[i] & 0xFC) >> 2;
            stream << table [b];
            b = (bytes[i] & 0x03) << 4;

            if (i + 1 < bytes.size())
            {
                b |= (bytes[i + 1] & 0xF0) >> 4;
                stream << table [b];
                b = (bytes[i + 1] & 0x0F) << 2;

                if (i + 2 < bytes.size())
                {
                    b |= (bytes[i + 2] & 0xC0) >> 6;
                    stream << table [b];
                    b = bytes[i + 2] & 0x3F;
                    stream << table [b];
                }
                else
                {
                    stream << table [b];
                    stream << '=';
                }
            }
            else
            {
                stream << table [b];
                stream << "==";
            }
        }
    }

    template <template <typename T, typename = std::allocator <T> > class ContainerT>
    void decodeBase64(std::string const& input, ContainerT <char>& bytes)
    {
        bytes.clear();

        if (input.empty())
            return;

        if (input.length() % 4 != 0)    {
            throw std::invalid_argument ("input does not have correct size for base64");
        }

        std::size_t size = (input.length() * 3) / 4;
        if (*input.rbegin() == '=')
            size--;
        if (*(input.rbegin() + 1) == '=')
            size--;

        bytes.resize(size);

        auto backwardsTable = [](int c) -> int {
            if (c >= (int)'A' && c <= (int)'Z')
                return c - 'A';
            if (c >= (int)'a' && c <= (int)'z')
                return c - 'a' + 26;
            if (c >= (int)'0' && c <= (int)'9')
                return c - '0' + 52;
            if (c == '+')
                return 62;
            if (c == '/')
                return 63;
            else
                std::invalid_argument ("input contains characters that are not base64");
            return 0;
        };

        int j = 0;
        int b[4];
        for (std::size_t i = 0; i < input.length(); i += 4)
        {
            b[0] = backwardsTable(input[i]);
            b[1] = backwardsTable(input[i + 1]);
            b[2] = backwardsTable(input[i + 2]);
            b[3] = backwardsTable(input[i + 3]);
            bytes[j++] = (char) ((b[0] << 2) | (b[1] >> 4));
            if (b[2] < 64)
            {
                bytes[j++] = (char) ((b[1] << 4) | (b[2] >> 2));
                if (b[3] < 64)
                {
                    bytes[j++] = (char) ((b[2] << 6) | b[3]);
                }
            }
        }
    }
}

#endif // BASE64_H_INCLUDED