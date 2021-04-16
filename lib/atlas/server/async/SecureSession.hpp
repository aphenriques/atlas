//
//  server/async/SecureSession.hpp
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

#ifndef atlas_server_async_SecureSession_hpp
#define atlas_server_async_SecureSession_hpp

#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include "SessionBase.hpp"

namespace atlas::server::async {
    class SecureSession : public SessionBase<SecureSession, boost::beast::ssl_stream<boost::beast::tcp_stream>> {
    public:
        SecureSession(boost::asio::ip::tcp::socket &&socket, boost::asio::ssl::context &sslContext);

        template<typename OnHandshake>
        void asyncHandshake(OnHandshake &&onHandshake);

        template<typename OnShutdown>
        void asyncShutdown(OnShutdown &&onShutdown);
    };

    //--

    template<typename OnHandshake>
    void SecureSession::SecureSession::asyncHandshake(OnHandshake &&onHandshake) {
       getStream().async_handshake(
            boost::asio::ssl::stream_base::server,
            [
                sessionSharedPointer = this->shared_from_this(),
                // attention! forwarded onHandshake
                onHandshakeCopy = std::forward<OnHandshake>(onHandshake)
            ](boost::system::error_code errorCode) {
                onHandshakeCopy(*sessionSharedPointer, errorCode);
            }
        );
    }

    template<typename OnShutdown>
    void SecureSession::asyncShutdown(OnShutdown &&onShutdown) {
        getStream().async_shutdown(
            [
                sessionSharedPointer = this->shared_from_this(),
                // attention! forwarded onShutdown
                onCloseCopy = std::forward<OnShutdown>(onShutdown)
            ](boost::system::error_code errorCode) {
                onCloseCopy(errorCode);
            }
        );
    }
}

#endif
