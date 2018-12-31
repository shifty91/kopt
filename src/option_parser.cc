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

#include <sstream>
#include <iomanip>
#include <algorithm>

#include <getopt.h>
#include <libgen.h>

#include <kopt/option_parser.h>
#include <kopt/flag_option.h>
#include <kopt/argument_option.h>
#include <kopt/multi_argument_option.h>
#include <kopt/unknown_option_exception.h>
#include <kopt/invalid_value_exception.h>
#include <kopt/missing_argument_exception.h>
#include <kopt/missing_required_option_exception.h>

namespace Kopt {

void OptionParser::add_flag_option(
    const std::string& name, const std::string& desc, const char short_name,
    const bool required)
{
    auto ptr = std::make_shared<FlagOption>(name, desc, short_name, required);
    options_[name] = ptr;
    s_options_[short_name] = ptr;
}

void OptionParser::add_argument_option(
    const std::string& name, const std::string& desc, const char short_name,
    const bool required, ValidFunc valid_func)
{
    auto ptr = std::make_shared<ArgumentOption>(
        name, desc, short_name, required, valid_func);
    options_[name] = ptr;
    s_options_[short_name] = ptr;
}

void OptionParser::add_multi_argument_option(
    const std::string& name, const std::string& desc, const char short_name,
    const bool required, ValidFunc valid_func)
{
    auto ptr = std::make_shared<MultiArgumentOption>(
        name, desc, short_name, required, valid_func);
    options_[name] = ptr;
    s_options_[short_name] = ptr;
}

std::vector<option> OptionParser::construct_longopts() const
{
    std::vector<option> res;
    struct option null = { };

    for (auto&& opt: options_)
        res.push_back(opt.second->to_long_opt());
    res.push_back(null);

    return res;
}

std::string OptionParser::get_usage(const std::string& additional_usage) const
{
    using namespace std::string_literals;

    std::stringstream ss;

    ss << "usage: ";
    ss << basename(argv_[0]);
    ss << " [options]";
    if (!additional_usage.empty())
        ss << " " << additional_usage;
    ss << std::endl;

    auto max = std::max_element(options_.begin(), options_.end(),
                                [] (const auto& a, const auto& b)
                                {
                                    return a.second->name().size() <
                                        b.second->name().size();
                                });
    auto max_len = max->second->name().size();

    for (auto&& opt: options_) {
        auto opt_str = "  --"s;
        opt_str += opt.second->name();
        opt_str += ", -";
        opt_str += opt.second->short_name();
        opt_str += ":";

        ss << std::left << std::setw(max_len + 9) << opt_str << " " << opt.second->desc()
           << std::endl;
    }

    return ss.str();
}

std::string OptionParser::construct_shortopts() const
{
    std::string s{":"};

    for (auto&& opt: options_)
        s += opt.second->to_short_opt();

    return s;
}

void OptionParser::parse()
{
    const auto short_opts = construct_shortopts();
    const auto long_opts  = construct_longopts();
    int c;

    while ((c = getopt_long(argc_, argv_, short_opts.c_str(),
                            long_opts.data(), NULL)) != -1) {
        // not found
        const auto it = s_options_.find(c);
        // missing argument
        if (c == ':')
            throw MissingArgumentException();
        if (c == '?' || it == s_options_.end())
            throw UnknownOptionException();
        // save
        it->second->consume(optarg == nullptr ? "1" : optarg);
    }

    for (auto&& opt: options_) {
        // check for required options
        if (opt.second->required() && !opt.second->consumed())
            throw MissingRequiredOptionException(opt.second->name());
        // not in valid range
        if (opt.second->consumed() && !opt.second->valid())
            throw InvalidValueException(*opt.second);
    }

    // add unparsed options
    const auto argc = argc_ - optind;
    const auto argv = argv_ + optind;

    for (auto i = 0; i < argc; ++i)
        unparsed_options_.emplace_back(argv[i]);
}

}
