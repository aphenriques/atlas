//
//  clientSync/main.cpp
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

#include <cstdlib>
#include <iostream>
#include <exception/Exception.hpp>
#include <atlas/client/sync/Connection.hpp>
#include <atlas/client/sync/Client.hpp>
#include <atlas/client/sync/SecureConnection.hpp>
#include <atlas/Url.hpp>

template<typename Connection, typename ...ExtraConnectionArguments>
void request(const atlas::Url &url, boost::asio::io_context &ioContext, ExtraConnectionArguments &&...extraConnectionArguments) {
    atlas::client::sync::Client<Connection> client(
        ioContext,
        url,
        std::forward<ExtraConnectionArguments>(extraConnectionArguments)...
    );
    boost::beast::http::request<boost::beast::http::string_body> request(boost::beast::http::verb::get, url.getTarget(), 11);
    // HTTP 1.1 requires the Host field
    // https://serverfault.com/a/163655
    request.set(boost::beast::http::field::host, url.getHost());
    std::cout << "request:\n" << request << "--" << std::endl;
    std::cout << "response:\n" << client.request(request) << "--" << std::endl;
    boost::system::error_code errorCode;
    client.shutdown(errorCode);
    if (errorCode.value() != 0) {
        std::cout << "shutdown error: " << errorCode.message() << std::endl;
    }
}

int main(int argc, char** argv) {
    try {
        if (argc == 2) {
            boost::asio::io_context ioContext;
            boost::asio::ssl::context sslContext(boost::asio::ssl::context::sslv23_client);
            sslContext.set_verify_mode(boost::asio::ssl::verify_none);
            const atlas::Url url(argv[1]);
            if (url.getScheme() == "http") {
                request<atlas::client::sync::Connection>(url, ioContext);
            } else if (url.getScheme() == "https") {
                request<atlas::client::sync::SecureConnection>(url, ioContext, sslContext);
            } else {
                throw exception::RuntimeException(__FILE__, __LINE__, __func__, "unexpected scheme: ", url.getScheme());
            }
            return EXIT_SUCCESS;
        } else {
            std::cerr << "usage: clientSync <url>" << std::endl;
            return EXIT_FAILURE;
        }
    } catch (const std::exception &exception) {
        std::cerr << "Error: " << exception.what() << std::endl;
        return EXIT_FAILURE;
    }
}
