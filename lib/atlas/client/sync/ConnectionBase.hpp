//
//  client/sync/ConnectionBase.hpp
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

#ifndef atlas_client_sync_ConnectionBase_hpp
#define atlas_client_sync_ConnectionBase_hpp

#include <boost/asio/socket_base.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/stream_traits.hpp>
#include <boost/beast/http.hpp>
#include "Exception.hpp"

namespace atlas::client::sync {
    template<typename StreamContainer>
    class ConnectionBase {
    public:
        template<typename Body, typename Fields>
        boost::beast::http::response<boost::beast::http::dynamic_body> request(
            boost::beast::flat_buffer &buffer,
            const boost::beast::http::request<Body, Fields> &requestMessage
        );
    };

    //--

    template<typename StreamContainer>
    template< typename Body, typename Fields>
    boost::beast::http::response<boost::beast::http::dynamic_body> ConnectionBase<StreamContainer>::request(
        boost::beast::flat_buffer &buffer,
        const boost::beast::http::request<Body, Fields> &requestMessage
    ) {
        auto &stream = static_cast<StreamContainer *>(this)->getStream();
        boost::system::error_code errorCode;
        boost::beast::http::write(stream, requestMessage, errorCode);
        if (errorCode.value() == 0) {
            boost::beast::http::response<boost::beast::http::dynamic_body> response;
            boost::beast::http::read(stream, buffer, response, errorCode);
            if (errorCode.value() == 0) {
                return response;
            } else {
                throw Exception(__FILE__, __LINE__, __func__, "read error: [[ ", errorCode.message(), " ]]");
            }
        } else {
            throw Exception(__FILE__, __LINE__, __func__, "write error: [[ ", errorCode.message(), " ]]");
        }
    }
}

#endif

