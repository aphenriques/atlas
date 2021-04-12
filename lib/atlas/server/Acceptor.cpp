//
//  server/Acceptor.cpp
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

#include "Acceptor.hpp"
#include <exception/Exception.hpp>

namespace atlas::server {
    Acceptor::Acceptor(boost::asio::io_context &ioContext, boost::asio::ip::tcp protocol, int port) :
        endpoint_(protocol, port),
        acceptor_(ioContext)
    {
        boost::system::error_code errorCode;
        acceptor_.open(endpoint_.protocol(), errorCode);
        if (errorCode.value() == 0) {
            // Allow address reuse (https://stackoverflow.com/a/3233022)
            acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true), errorCode);
            if (errorCode.value() == 0) {
                acceptor_.set_option(boost::asio::ip::tcp::acceptor::keep_alive(true), errorCode);
                if (errorCode.value() == 0) {
                    acceptor_.bind(endpoint_, errorCode);
                    if (errorCode.value() == 0) {
                        // Start listening for connections
                        acceptor_.listen(boost::asio::socket_base::max_listen_connections, errorCode);
                        if (errorCode.value() == 0) {
                            return;
                        } else {
                            throw exception::RuntimeException(__FILE__, __LINE__, __func__, "acceptor listen error: [[ ", errorCode.message(), " ]]");
                        }
                    } else {
                        throw exception::RuntimeException(__FILE__, __LINE__, __func__, "acceptor bind error: [[ ", errorCode.message(), " ]]");
                    }
                } else {
                    throw exception::RuntimeException(__FILE__, __LINE__, __func__, "acceptor keep_alive option error: [[ ", errorCode.message(), " ]]");
                }
            } else {
                throw exception::RuntimeException(__FILE__, __LINE__, __func__, "acceptor reuse_address option error: [[ ", errorCode.message(), " ]]");
            }
        } else {
            throw exception::RuntimeException(__FILE__, __LINE__, __func__, "acceptor open error: [[ ", errorCode.message(), " ]]");
        }
    }

    boost::asio::ip::tcp::acceptor & Acceptor::getAcceptor() {
        return acceptor_;
    }
}
