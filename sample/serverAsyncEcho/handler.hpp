//
//  serverAsyncEcho/handler.hpp
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

#ifndef handler_hpp
#define handler_hpp

#include <iostream>
#include <thread>
#include <exception/Exception.hpp>
#include <atlas/server/async/SecureSession.hpp>
#include <atlas/server/async/Server.hpp>
#include <atlas/server/async/Session.hpp>

namespace handler {
    namespace global {
        extern std::mutex outputMutex;
    }

    template<typename SocketHandler>
    class OnAccept {
    public:
        OnAccept(const SocketHandler &socketHandler);

        void operator()(
            atlas::server::async::Server &server,
            boost::system::error_code errorCode,
            boost::asio::ip::tcp::socket &&socket
        ) const;

    private:
        SocketHandler socketHandler_;
    };

    void startSession(boost::asio::ip::tcp::socket &&socket);

    class SecureSessionStarter {
    public:
        SecureSessionStarter(boost::asio::ssl::context &sslContextReference);

        void operator()(boost::asio::ip::tcp::socket &&socket) const;

    private:
        boost::asio::ssl::context &sslContextReference_;
    };

    void onHandshake(atlas::server::async::SecureSession &secureSession, boost::system::error_code errorCode);

    template<typename Session>
    void onRead(Session &session, boost::system::error_code errorCode, std::size_t bytesTransferred);

    template<typename Session>
    void onWrite(
        Session &session,
        bool eofNeeded,
        boost::system::error_code errorCode,
        std::size_t bytesTransferred
    );

    void close(atlas::server::async::Session &session);
    void close(atlas::server::async::SecureSession &secureSession);
    void onClose(boost::system::error_code errorCode);

    //--

    template<typename SocketHandler>
    OnAccept<SocketHandler>::OnAccept(const SocketHandler &socketHandler) :
        socketHandler_(socketHandler)
    {}

    template<typename SocketHandler>
    void OnAccept<SocketHandler>::operator()(
        atlas::server::async::Server &server,
        boost::system::error_code errorCode,
        boost::asio::ip::tcp::socket &&socket
    ) const {
        if (errorCode.value() == 0) {
            // attention! moved socket
            socketHandler_(std::move(socket));
            server.asyncAccept(OnAccept<SocketHandler>(socketHandler_));
            std::lock_guard<std::mutex> lockGuard(global::outputMutex);
            std::cout << "connected" << std::endl;
        } else {
            throw exception::RuntimeException(__FILE__, __LINE__, __func__, "accept error: [[ ", errorCode.message(), " ]]");
        }
    }

    template<typename Session>
    void onRead(Session &session, boost::system::error_code errorCode, std::size_t) {
        if (errorCode.value() == 0) {
            {
                std::lock_guard<std::mutex> lockGuard(global::outputMutex);
                std::cout << session.getHttpRequest() << std::endl;
            }
            boost::beast::http::response<boost::beast::http::string_body> response;
            response.version(11);
            response.result(boost::beast::http::status::ok);
            response.keep_alive(session.getHttpRequest().keep_alive());
            response.set(boost::beast::http::field::server, "httpAsyncServerEcho");
            response.body() = session.getHttpRequest().body();
            response.prepare_payload();
            // attention! moved response
            session.asyncWrite(std::move(response), onWrite<Session>);
        } else if (errorCode == boost::beast::http::error::end_of_stream) {
            close(session);
        } else {
            std::lock_guard<std::mutex> lockGuard(global::outputMutex);
            std::cout << "ignored read error: " << errorCode.message() << std::endl;
        }
    }

    template<typename Session>
    void onWrite(Session &session, bool eofNeeded, boost::system::error_code errorCode, std::size_t) {
        if (errorCode.value() == 0) {
            if (eofNeeded == false) {
                session.asyncRead(onRead<Session>);
                return;
            } else {
                close(session);
            }
        } else {
            throw exception::RuntimeException(__FILE__, __LINE__, __func__, "write error: [[ ", errorCode.message(), " ]]");
        }
    }
}

#endif
