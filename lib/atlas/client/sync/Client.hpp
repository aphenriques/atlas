//
//  client/sync/Client.hpp
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

#ifndef atlas_client_sync_Client_hpp
#define atlas_client_sync_Client_hpp

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http.hpp>
#include "Exception.hpp"
#include "../../Url.hpp"

namespace atlas::client::sync {
    template<typename Connection>
    class Client {
    public:
        Client(const Client &) = delete;
        Client & operator=(const Client &) = delete;

        template<typename ...SslContextOrNothing>
        Client(
            boost::asio::io_context &ioContext,
            const Url &url,
            SslContextOrNothing &...sslContextOrNothing
        );

        template<typename Body, typename Fields>
        boost::beast::http::response<boost::beast::http::dynamic_body> request(
            const boost::beast::http::request<Body, Fields> &request
        );

        boost::system::error_code shutdown();

    private:
        boost::asio::ip::tcp::resolver resolver_;
        Connection connection_;
        boost::beast::flat_buffer buffer_;
    };

    //--

    template<typename Connection>
    template<typename ...SslContextOrNothing>
    Client<Connection>::Client(
        boost::asio::io_context &ioContext,
        const Url &url,
        SslContextOrNothing &...sslContextOrNothing
    ) :
        resolver_(ioContext),
        connection_(ioContext, resolver_, url, sslContextOrNothing...)
    {}

    template<typename Connection>
    template<typename Body, typename Fields>
    boost::beast::http::response<boost::beast::http::dynamic_body> Client<Connection>::request(
        const boost::beast::http::request<Body, Fields> &requestMessage
    ) {
        boost::system::error_code errorCode;
        boost::beast::http::write(connection_.getStream(), requestMessage, errorCode);
        if (errorCode.value() == 0) {
            boost::beast::http::response<boost::beast::http::dynamic_body> response;
            boost::beast::http::read(connection_.getStream(), buffer_, response, errorCode);
            if (errorCode.value() == 0) {
                return response;
            } else {
                throw Exception(__FILE__, __LINE__, __func__, "read error: [[ ", errorCode.message(), " ]]");
            }
        } else {
            throw Exception(__FILE__, __LINE__, __func__, "write error: [[ ", errorCode.message(), " ]]");
        }
    }

    template<typename Connection>
    boost::system::error_code Client<Connection>::shutdown() {
        return connection_.shutdown();
    }
}

#endif
