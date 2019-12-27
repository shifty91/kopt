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

#ifndef _OPTION_PARSER_H_
#define _OPTION_PARSER_H_

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>

#include <kopt/option.h>
#include <kopt/flag_option.h>
#include <kopt/argument_option.h>
#include <kopt/multi_argument_option.h>
#include <kopt/unknown_option_exception.h>

namespace Kopt {

class OptionParser
{
public:
    OptionParser(int argc, char **argv) :
        argc_{argc}, argv_{argv}
    {}

    void add_flag_option(
        const std::string& name, const std::string& desc,
        const char short_name, const bool required = false)
    {
        add_option<FlagOption>(name, desc, short_name, required);
    }

    void add_argument_option(
        const std::string& name, const std::string& desc,
        const char short_name, const bool required = false,
        ValidFunc valid_func = [] (const Option&) -> bool { return true; })
    {
        add_option<ArgumentOption>(name, desc, short_name, required, valid_func);
    }

    void add_multi_argument_option(
        const std::string& name, const std::string& desc,
        const char short_name, const bool required = false,
        ValidFunc valid_func = [] (const Option&) -> bool { return true; })
    {
        add_option<MultiArgumentOption>(name, desc, short_name, required, valid_func);
    }

    void parse();

    std::string get_usage(const std::string& additonal_usage = "") const;

    std::shared_ptr<Option> operator[](const std::string& opt)
    {
        auto it = options_.find(opt);
        if (it == options_.end())
            throw UnknownOptionException(opt);
        return it->second;
    }

    const std::vector<std::string>& unparsed_options() const
    {
        return unparsed_options_;
    }

private:
    std::vector<option> construct_longopts() const;
    std::string construct_shortopts() const;

    template<typename OPTION>
    void add_option(
        const std::string& name, const std::string& desc,
        const char short_name, const bool required = false,
        ValidFunc valid_func = [] (const Option&) -> bool { return true; })
    {

        auto ptr = std::make_shared<OPTION>(
            name, desc, short_name, required, valid_func);
        options_[name] = ptr;
        s_options_[short_name] = ptr;
    }

    int argc_;
    char **argv_;
    std::map<std::string, std::shared_ptr<Option>> options_;
    std::map<char, std::shared_ptr<Option>> s_options_;
    std::vector<std::string> unparsed_options_;
};

}

#endif /* _OPTION_PARSER_H_ */
