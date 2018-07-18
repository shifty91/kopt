// Copyright 2018 Kurt Kanzenbach <kurt@kmk-computers.de>
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

#ifndef _ARGUMENT_OPTION_H_
#define _ARGUMENT_OPTION_H_

#include <string>
#include <stdexcept>
#include <functional>

#include <unistd.h>
#include <getopt.h>

#include <kopt/option.h>

namespace Kopt {

class ArgumentOption final: public Option
{
public:
    ArgumentOption(const std::string name, const std::string desc, const char short_name,
                   const bool required = false,
                   std::function<bool(const Option&)> valid_func =
                   [] (const Option&) -> bool { return true; }) :
        Option(name, desc, short_name, required, valid_func)
    {}

    virtual ~ArgumentOption()
    {}

    virtual struct option to_long_opt() const override
    {
        return { name_.c_str(), required_argument, nullptr, short_name_ };
    }

    virtual std::string to_short_opt() const override
    {
        std::string s{short_name_};
        s += ":";
        return s;
    }

    virtual void consume(const std::string& arg) override
    {
        value_ = arg;
        consumed_ = true;
    }

    std::string& value() noexcept
    {
        return value_;
    }

    const std::string& value() const noexcept
    {
        return value_;
    }
};

}

#endif /* _ARGUMENT_OPTION_H_ */
