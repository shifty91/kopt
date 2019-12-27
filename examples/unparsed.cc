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

#include <iostream>
#include <kopt/kopt.h>

using namespace Kopt;

int main(int argc, char *argv[])
{
    OptionParser parser{argc, argv};

    parser.add_flag_option("debug", "Enable debug output", 'd');

    try {
        parser.parse();
        if (parser["debug"]->to<bool>())
            std::cout << "Debug set!" << std::endl;
        std::cout << "Unparsed options: [";
        for (auto i = 0u; i < parser.unparsed_options().size(); ++i) {
            std::cout << parser.unparsed_options()[i];
            if (i != (parser.unparsed_options().size() - 1))
                std::cout << ",";
        }
        std::cout << "]" << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Failed to parse arguments: " << ex.what() << std::endl;
        std::cerr << "Printing usage:" << std::endl;
        std::cout << parser.get_usage();
    }

    return 0;
}
