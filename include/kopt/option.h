// Copyright 2018,2019 Kurt Kanzenbach <kurt@kmk-computers.de>
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef _OPTION_H_
#define _OPTION_H_

#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <functional>
#include <vector>
#include <memory>

#include <unistd.h>
#include <getopt.h>

#include <kopt/conversion_exception.h>

namespace Kopt {

class Option;

using ValidFunc = std::function<bool(const Option&)>;

class Option
{
public:
    friend std::ostream& operator<< (std::ostream& os, const Option& opt);

    Option(const std::string& name, const std::string& desc,
           const char short_name, const bool required = false,
           ValidFunc valid_func = [] (const Option&) -> bool { return true; }) :
        name_{name}, desc_{desc}, short_name_{short_name},
        required_{required}, valid_func_{valid_func}, consumed_{false}
    {}

    virtual ~Option()
    {}

    virtual struct option to_long_opt() const = 0;

    virtual std::string to_short_opt() const = 0;

    virtual void consume(const std::string& arg) = 0;

    bool valid() const
    {
        if (sub_options_.empty()) {
            return valid_func_(*this);
        } else {
            for (auto&& sub_opt: sub_options_)
                if (!valid_func_(*sub_opt))
                    return false;
        }
        return true;
    }

    const std::string& value() const noexcept
    {
        return value_;
    }

    std::string& value() noexcept
    {
        return value_;
    }

    const std::string& name() const noexcept
    {
        return name_;
    }

    std::string& name() noexcept
    {
        return name_;
    }

    const std::string& desc() const noexcept
    {
        return desc_;
    }

    std::string& desc() noexcept
    {
        return desc_;
    }

    const std::string& to() const noexcept
    {
        return value_;
    }

    const char& short_name() const noexcept
    {
        return short_name_;
    }

    char& short_name() noexcept
    {
        return short_name_;
    }

    const bool& required() const noexcept
    {
        return required_;
    }

    bool& required() noexcept
    {
        return required_;
    }

    const bool& consumed() const noexcept
    {
        return consumed_;
    }

    bool& consumed() noexcept
    {
        return consumed_;
    }

    auto begin()
    {
        return sub_options_.begin();
    }

    auto begin() const
    {
        return sub_options_.begin();
    }

    auto cbegin() const
    {
        return sub_options_.cbegin();
    }

    auto end()
    {
        return sub_options_.end();
    }

    auto end() const
    {
        return sub_options_.end();
    }

    auto cend() const
    {
        return sub_options_.cend();
    }

    template<typename T>
    T to() const
    {
        T res;
        std::stringstream ss{value_};

        static_assert(std::is_arithmetic_v<T>,
                      "Option can only be converted to arithmetic type!");

        if (!(ss >> res))
            throw ConversionException(value_);

        return res;
    }

    operator bool() const noexcept
    {
        return consumed_;
    }

    std::string to_string() const
    {
        std::stringstream ss;

        ss << "[";
        if (sub_options_.empty()) {
            ss << value_;
        } else {
            for (auto i = 0u; i < sub_options_.size(); ++i) {
                ss << sub_options_.at(i)->value();
                if (i != sub_options_.size() - 1)
                    ss << ",";
            }
        }
        ss << "]";

        return ss.str();
    }

protected:
    std::string value_;
    std::string name_;
    std::string desc_;
    char short_name_;
    bool required_;
    ValidFunc valid_func_;
    bool consumed_;
    std::vector<std::shared_ptr<Option>> sub_options_;
};

inline std::ostream& operator<< (std::ostream& os, const Option& opt)
{
    os << opt.to_string();
    return os;
}

}

#endif /* _OPTION_H_ */
