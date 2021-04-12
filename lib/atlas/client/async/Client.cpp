//
//  client/async/Client.cpp
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

#include "Client.hpp"
#include <boost/beast/version.hpp>
#include <exception/Exception.hpp>

// FIXME
#include <iostream>

namespace atlas::client::async {
    Client::Client(boost::asio::io_context &ioContext, boost::asio::ssl::context &sslContext) :
        resolver_(ioContext),
        sslStream_(ioContext, sslContext)
    {}

    void Client::run(char const* host, char const* port, char const* target, int version) {
        // Set SNI Hostname (many hosts need this to handshake successfully)
        if (SSL_set_tlsext_host_name(sslStream_.native_handle(), host)) {
            // Set up an HTTP GET request message
            req_.version(version);
            req_.method(boost::beast::http::verb::get);
            req_.target(target);
            req_.set(boost::beast::http::field::host, host);
            req_.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            // Look up the domain name
            resolver_.async_resolve(
                host,
                port,
                boost::beast::bind_front_handler(
                    &Client::on_resolve,
                    shared_from_this()
                )
            );
        } else {
            throw exception::RuntimeException(__FILE__, __LINE__, __func__, "SSL_set_tlsext_host_name error: [[ ", boost::system::error_code(static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()).message(), " ]]");
        }
    }

    void Client::on_resolve(boost::system::error_code errorCode, boost::asio::ip::tcp::resolver::results_type results) {
        if (errorCode.value() == 0) {
            // Set a timeout on the operation
            boost::beast::get_lowest_layer(sslStream_).expires_after(std::chrono::seconds(30));
            // Make the connection on the IP address we get from a lookup
            boost::beast::get_lowest_layer(sslStream_).async_connect(
                results,
                boost::beast::bind_front_handler(
                    &Client::on_connect,
                    shared_from_this()
                )
            );
        } else {
            throw exception::RuntimeException(__FILE__, __LINE__, __func__, "resolve error: [[ ", errorCode.message(), " ]]");
        }
    }

    void Client::on_connect(boost::system::error_code errorCode, boost::asio::ip::tcp::resolver::results_type::endpoint_type) {
        if (errorCode.value() == 0) {
            // Perform the SSL handshake
            sslStream_.async_handshake(
                boost::asio::ssl::stream_base::client,
                boost::beast::bind_front_handler(
                    &Client::on_handshake,
                    shared_from_this()
                )
            );
        } else {
            throw exception::RuntimeException(__FILE__, __LINE__, __func__, "connect error: [[ ", errorCode.message(), " ]]");
        }
    }

    void Client::on_handshake(boost::system::error_code errorCode) {
        if (errorCode.value() == 0) {
            // Set a timeout on the operation
            boost::beast::get_lowest_layer(sslStream_).expires_after(std::chrono::seconds(30));
            // Send the HTTP request to the remote host
            boost::beast::http::async_write(
                sslStream_,
                req_,
                boost::beast::bind_front_handler(
                    &Client::on_write,
                    shared_from_this()
                )
            );
        } else {
            throw exception::RuntimeException(__FILE__, __LINE__, __func__, "handshake error: [[ ", errorCode.message(), " ]]");
        }
    }

    void Client::on_write(boost::system::error_code errorCode, std::size_t) {
        if (errorCode.value() == 0) {
            // Receive the HTTP response
            boost::beast::http::async_read(
                sslStream_,
                buffer_,
                res_,
                boost::beast::bind_front_handler(
                    &Client::on_read,
                    shared_from_this()
                )
            );
        } else {
            throw exception::RuntimeException(__FILE__, __LINE__, __func__, "write error: [[ ", errorCode.message(), " ]]");
        }
    }

    void Client::on_read(boost::system::error_code errorCode, std::size_t) {
        if (errorCode.value() == 0) {
            // Write the message to standard out
            std::cout << res_ << std::endl;
            // Set a timeout on the operation
            boost::beast::get_lowest_layer(sslStream_).expires_after(std::chrono::seconds(30));
            // Gracefully close the stream
            sslStream_.async_shutdown(
                boost::beast::bind_front_handler(
                    &Client::on_shutdown,
                    shared_from_this()
                )
            );
        } else {
            throw exception::RuntimeException(__FILE__, __LINE__, __func__, "read error: [[ ", errorCode.message(), " ]]");
        }
    }

    void Client::on_shutdown(boost::system::error_code errorCode) {
        if (errorCode != boost::asio::error::eof) {
            // Rationale:
            // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
            throw exception::RuntimeException(__FILE__, __LINE__, __func__, "shutdown error: [[ ", errorCode.message(), " ]]");
        }
    }
}
