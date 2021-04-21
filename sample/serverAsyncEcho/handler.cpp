//
//  serverAsyncEcho/handler.cpp
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

#include "handler.hpp"

namespace handler {
    namespace global {
        std::mutex outputMutex;
    }

    void startSession(boost::asio::ip::tcp::socket &&socket) {
        using Session = atlas::server::async::Session;
        std::make_shared<Session>(std::move(socket))->asyncRead(onRead<Session>);
    }

    SecureSessionStarter::SecureSessionStarter(boost::asio::ssl::context &sslContextReference) : sslContextReference_(sslContextReference) {}

    void SecureSessionStarter::operator()(boost::asio::ip::tcp::socket &&socket) const {
        std::make_shared<atlas::server::async::SecureSession>(std::move(socket), sslContextReference_)->asyncHandshake(onHandshake);
    }

    void onHandshake(atlas::server::async::SecureSession &secureSession, boost::system::error_code errorCode) {
        if (errorCode.value() == 0) {
            secureSession.asyncRead(onRead<atlas::server::async::SecureSession>);
        } else {
            {
                std::lock_guard<std::mutex> lockGuard(global::outputMutex);
                std::cout << "ignored handshake error: " << errorCode.message() << std::endl;
            }
            secureSession.asyncShutdown(onShutdown);
        }
    }

    void shutdown(atlas::server::async::Session &session) {
        boost::system::error_code errorCode;
        session.shutdown(errorCode);
        onShutdown(errorCode);
    }

    void shutdown(atlas::server::async::SecureSession &secureSession) {
        secureSession.asyncShutdown(onShutdown);
    }

    void onShutdown(boost::system::error_code errorCode) {
        if (errorCode.value() == 0) {
            std::lock_guard<std::mutex> lockGuard(global::outputMutex);
            std::cout << "session shutdown" << std::endl;
        } else {
            std::lock_guard<std::mutex> lockGuard(global::outputMutex);
            std::cout << "ignored shutdown error: " << errorCode.message() << std::endl;
        }
    }
}
