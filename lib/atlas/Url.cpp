//
//  Url.cpp
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

#include "Url.hpp"
#include <exception/Exception.hpp>

namespace atlas {
    Url::Url(const std::string &url) try :
        Url(getParseURL(url))
    {
    } catch (const std::exception &exception) {
        throw exception::RuntimeException(__FILE__, __LINE__, __func__, "url parse error [[ ", exception.what(), " ]] on url [[ ", url, " ]]");
    }

    const std::string & Url::getScheme() const {
        return scheme_;
    }

    const std::string & Url::getHost() const {
        return host_;
    }

    const std::string & Url::getPort() const {
        return port_;
    }

    const std::string & Url::getTarget() const {
        return target_;
    }

    LUrlParser::ParseURL Url::getParseURL(const std::string &url) {
        LUrlParser::ParseURL parseURLObject = LUrlParser::ParseURL::parseURL(url);
        if (parseURLObject.isValid() == true) {
            return parseURLObject;
        } else {
            const auto getParseErrorString = [](LUrlParser::LUrlParserError lUrlParserError) {
                switch (lUrlParserError) {
                    case LUrlParser::LUrlParserError_Ok:
                        return "LUrlParserError_Ok";
                    case LUrlParser::LUrlParserError_Uninitialized:
                        return "LUrlParserError_Uninitialized";
                    case LUrlParser::LUrlParserError_NoUrlCharacter:
                        return "LUrlParserError_NoUrlCharacter";
                    case LUrlParser::LUrlParserError_InvalidSchemeName:
                        return "LUrlParserError_InvalidSchemeName";
                    case LUrlParser::LUrlParserError_NoDoubleSlash:
                        return "LUrlParserError_NoDoubleSlash";
                    case LUrlParser::LUrlParserError_NoAtSign:
                        return "LUrlParserError_NoAtSign";
                    case LUrlParser::LUrlParserError_UnexpectedEndOfLine:
                        return "LUrlParserError_UnexpectedEndOfLine";
                    case LUrlParser::LUrlParserError_NoSlash:
                        return "LUrlParserError_NoSlash";
                }
                return "undefined error";
            };
            throw exception::RuntimeException(__FILE__, __LINE__, __func__, "parse error: ", getParseErrorString(parseURLObject.errorCode_));
        }
    }

    Url::Url(const LUrlParser::ParseURL &parseURL) :
        Url(
            parseURL.scheme_,
            parseURL.host_,
            [&parseURL]() -> std::string {
                if (parseURL.port_.empty() == true) {
                    if (parseURL.scheme_ == "http") {
                        return "80";
                    } else if (parseURL.scheme_ == "https") {
                        return "443";
                    } else {
                        throw exception::RuntimeException(__FILE__, __LINE__, __func__, "unexpected scheme: [[ ", parseURL.scheme_, " ]]");
                    }
                } else {
                    return parseURL.port_;
                }
            }(),
            [&parseURL] {
                std::string target = "/" + parseURL.path_;
                if (parseURL.query_.empty() == false) {
                    target += '?' + parseURL.query_;
                }
                if (parseURL.fragment_.empty() == false) {
                    target += '#' + parseURL.fragment_;
                }
                return target;
            }()
       )
    {}

    Url::Url(const std::string &scheme, const std::string &host, const std::string &port, const std::string &target) :
        scheme_(scheme),
        host_(host),
        port_(port),
        target_(target)
    {}
}
