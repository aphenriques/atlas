//
//  server/async/Server.hpp
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

#ifndef atlas_server_async_Server_hpp
#define atlas_server_async_Server_hpp

#include "../Acceptor.hpp"

namespace atlas::server::async {
    class Server {
    public:
        template<typename OnAccept>
        Server(
            boost::asio::io_context &ioContext,
            boost::asio::ip::tcp protocol,
            int port,
            OnAccept &&onAccept
        );

        template<typename OnAccept>
        void asyncAccept(OnAccept &&onAccept);

    private:
        Acceptor acceptor_;
    };

    //--

    template<typename OnAccept>
    Server::Server(
        boost::asio::io_context &ioContext,
        boost::asio::ip::tcp protocol,
        int port,
        OnAccept &&onAccept
    ) :
        acceptor_(ioContext, protocol, port)
    {
        // attention! forwarded onAccept
        asyncAccept(std::forward<OnAccept>(onAccept));
    }

    template<typename OnAccept>
    void Server::asyncAccept(OnAccept &&onAccept) {
        acceptor_.getAcceptor().async_accept(
            // attention! forwarded onAccept
            [this, onAcceptCopy = std::forward<OnAccept>(onAccept)](boost::system::error_code errorCode, boost::asio::ip::tcp::socket &&socket) {
                // attention! moved socket
                onAcceptCopy(*this, errorCode, std::move(socket));
            }
        );
    }
}

#endif
