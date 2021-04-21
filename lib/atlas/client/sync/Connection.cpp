//
//  client/sync/Connection.cpp
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

#include "Connection.hpp"
#include <boost/asio/connect.hpp>
#include "Exception.hpp"

namespace atlas::client::sync {
    Connection::Connection(
        boost::asio::io_context &ioContext,
        boost::asio::ip::tcp::resolver &resolver,
        const Url &url
    ) :
        stream_(ioContext)
    {
        boost::system::error_code errorCode;
        const auto results = resolver.resolve(url.getHost(), url.getPort(), errorCode);
        if (errorCode.value() == 0) {
            boost::beast::get_lowest_layer(stream_).connect(results, errorCode);
            if (errorCode.value() == 0) {
                return;
            } else {
                throw Exception(__FILE__, __LINE__, __func__, "connect error: [[ ", errorCode.message(), " ]]");
            }
        } else {
            throw Exception(__FILE__, __LINE__, __func__, "resolve error: [[ ", errorCode.message(), " ]]");
        }
    }

    boost::beast::tcp_stream & Connection::getStream() {
        return stream_;
    }

    void Connection::shutdown(boost::system::error_code &errorCode) {
        stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, errorCode);
    }
}
