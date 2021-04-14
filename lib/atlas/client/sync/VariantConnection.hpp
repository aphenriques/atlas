//
//  client/sync/VariantConnection.hpp
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

#ifndef atlas_client_sync_VariantConnection_hpp
#define atlas_client_sync_VariantConnection_hpp

#include <variant>
#include "Connection.hpp"
#include "SecureConnection.hpp"

namespace atlas::client::sync {
    class VariantConnection {
    public:
        VariantConnection(
            boost::asio::io_context &ioContext,
            boost::asio::ip::tcp::resolver &resolver,
            const Url &url,
            boost::asio::ssl::context &sslContext
        );

        template<typename Body, typename Fields>
        boost::beast::http::response<boost::beast::http::dynamic_body> request(
            boost::beast::flat_buffer &buffer,
            const boost::beast::http::request<Body, Fields> &requestMessage
        );

        boost::system::error_code shutdown();

    private:
        std::variant<std::monostate, SecureConnection, Connection> variantConnection_;
    };

    //--

    template<typename Body, typename Fields>
    boost::beast::http::response<boost::beast::http::dynamic_body> VariantConnection::request(
        boost::beast::flat_buffer &buffer,
        const boost::beast::http::request<Body, Fields> &requestMessage
    ) {
        return std::visit(
            [&buffer, &requestMessage](auto &&connection) -> boost::beast::http::response<boost::beast::http::dynamic_body> {
                if constexpr (std::is_same_v<std::decay_t<decltype(connection)>, std::monostate> == false) {
                    return connection.request(buffer, requestMessage);
                } else {
                    throw Exception(__FILE__, __LINE__, __func__, "undefined variantConnection_");
                }
            },
            variantConnection_
        );
    }
}

#endif
