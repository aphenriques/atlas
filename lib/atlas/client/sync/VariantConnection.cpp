//
//  client/sync/VariantConnection.cpp
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

#include "VariantConnection.hpp"

namespace atlas::client::sync {
    VariantConnection::VariantConnection(
        boost::asio::io_context &ioContext,
        boost::asio::ip::tcp::resolver &resolver,
        const Url &url,
        boost::asio::ssl::context &sslContext
    ) {
        if (url.getScheme() == "https") {
            variantConnection_.emplace<SecureConnection>(ioContext, resolver, url, sslContext);
        } else if (url.getScheme() == "http") {
            variantConnection_.emplace<Connection>(ioContext, resolver, url);
        } else {
            throw Exception(__FILE__, __LINE__, __func__, "unexpected url scheme: ", url.getScheme());
        }
    }

    void VariantConnection::shutdown(boost::system::error_code &errorCode) {
        return std::visit(
            [&errorCode](auto &&connection) {
                if constexpr (std::is_same_v<std::decay_t<decltype(connection)>, std::monostate> == false) {
                    connection.shutdown(errorCode);
                } else {
                    throw Exception(__FILE__, __LINE__, __func__, "undefined variantConnection_");
                }
            },
            variantConnection_
        );
    }
}
