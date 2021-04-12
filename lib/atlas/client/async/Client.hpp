//
//  client/async/Client.hpp
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

#ifndef atlas_client_async_Client_hpp
#define atlas_client_async_Client_hpp

#include <memory>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>

namespace atlas::client::async {
    // Performs an HTTP GET and prints the response
    class Client : public std::enable_shared_from_this<Client> {
        boost::asio::ip::tcp::resolver resolver_;
        boost::beast::ssl_stream<boost::beast::tcp_stream> sslStream_;
        boost::beast::flat_buffer buffer_; // (Must persist between reads)
        boost::beast::http::request<boost::beast::http::empty_body> req_;
        boost::beast::http::response<boost::beast::http::string_body> res_;

    public:
        explicit Client(boost::asio::io_context &ioContext, boost::asio::ssl::context &sslContext);

        void run(char const* host, char const* port, char const* target, int version);
        void on_resolve(boost::system::error_code errorCode, boost::asio::ip::tcp::resolver::results_type results);
        void on_connect(boost::system::error_code errorCode, boost::asio::ip::tcp::resolver::results_type::endpoint_type);
        void on_handshake(boost::system::error_code errorCode);
        void on_write(boost::system::error_code errorCode, std::size_t bytesTransfered);
        void on_read(boost::system::error_code errorCode, std::size_t bytesTransfered);
        void on_shutdown(boost::system::error_code errorCode);
    };
}

#endif

