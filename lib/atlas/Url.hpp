//
//  Url.hpp
//  atlas
//
// MIT License
//
// Copyright (c) 2021 André Pereira Henriques (aphenriques (at) outlook (dot) com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef atlas_Url_hpp
#define atlas_Url_hpp

#include <LUrlParser/LUrlParser.h>

namespace atlas {
    class Url {
    public:
        Url(const Url &) = delete;
        Url & operator=(const Url &) = delete;

        Url(const std::string &url);

        const std::string & getScheme() const;
        const std::string & getHost() const;
        const std::string & getPort() const;
        const std::string & getTarget() const;

    private:
        static LUrlParser::ParseURL getParseURL(const std::string &url);

        Url(const LUrlParser::ParseURL &parseURL);
        Url(const std::string &scheme, const std::string &host, const std::string &port, const std::string &target);

        const std::string scheme_;
        const std::string host_;
        const std::string port_;
        const std::string target_;
    };
}

#endif

