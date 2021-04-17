//
//  client/sync/SecureConnection.hpp
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

#ifndef atlas_client_sync_SecureConnection_hpp
#define atlas_client_sync_SecureConnection_hpp

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include "../../Url.hpp"
#include "ConnectionBase.hpp"

namespace atlas::client::sync {
    class SecureConnection : public ConnectionBase<SecureConnection> {
    public:
        SecureConnection(
            boost::asio::io_context &ioContext,
            boost::asio::ip::tcp::resolver &resolver,
            const Url &url,
            boost::asio::ssl::context &sslContext
        );

        boost::beast::ssl_stream<boost::beast::tcp_stream> & getStream();
        boost::system::error_code shutdown();

    private:
        boost::beast::ssl_stream<boost::beast::tcp_stream> stream_;
    };
}

#endif

