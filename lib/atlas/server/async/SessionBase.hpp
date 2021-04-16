//
//  server/async/SessionBase.hpp
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

#ifndef atlas_server_async_SessionBase_hpp
#define atlas_server_async_SessionBase_hpp

#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http.hpp>
#include <memory>

namespace atlas::server::async {
    template<typename Derived, typename StreamType>
    class SessionBase : public std::enable_shared_from_this<Derived> {
    public:
        boost::beast::http::request<boost::beast::http::string_body> & getHttpRequest();

        template<bool request, typename Body, typename Fields, typename OnWrite>
        void asyncWrite(boost::beast::http::message<request, Body, Fields> &&message, OnWrite &&onWrite);

        template<typename OnRead>
        void asyncRead(OnRead &&onRead);

    protected:
        SessionBase(StreamType &&stream);

        StreamType & getStream();

    private:
        StreamType stream_;
        boost::beast::flat_buffer buffer_;
        boost::beast::http::request<boost::beast::http::string_body> httpRequest_;
    };

    //--

    template<typename Derived, typename StreamType>
    boost::beast::http::request<boost::beast::http::string_body> & SessionBase<Derived, StreamType>::getHttpRequest() {
        return httpRequest_;
    }

    template<typename Derived, typename StreamType>
    template<bool request, typename Body, typename Fields, typename OnWrite>
    void SessionBase<Derived, StreamType>::asyncWrite(boost::beast::http::message<request, Body, Fields> &&message, OnWrite &&onWrite) {
        // attention! moved message
        auto messageSharedPointer = std::make_shared<boost::beast::http::message<request, Body, Fields>>(std::move(message));
        const boost::beast::http::message<request, Body, Fields> * const messageSharedPointerPointer = messageSharedPointer.get();
        boost::beast::http::async_write(
            stream_,
            // pass the pointer because messageSharedPointer might have been moved (invalidating it)
            *messageSharedPointerPointer,
            [
                thisSharedPointer = this->shared_from_this(),
                // attention! moved messageSharedPointer
                messageSharedPointerCopy = std::move(messageSharedPointer),
                // attention! forwarded onWrite
                onWriteCopy = std::forward<OnWrite>(onWrite)
            ](boost::system::error_code errorCode, std::size_t bytesTransferred) {
                onWriteCopy(*thisSharedPointer, errorCode, bytesTransferred);
            }
        );
    }

    template<typename Derived, typename StreamType>
    template<typename OnRead>
    void SessionBase<Derived, StreamType>::asyncRead(OnRead &&onRead) {
        // Make the request empty before reading, otherwise the operation behavior is undefined.
        httpRequest_ = {};
        boost::beast::http::async_read(
            stream_,
            buffer_,
            httpRequest_,
            [
                thisSharedPointer = this->shared_from_this(),
                // attention! forwarded onRead
                onReadCopy = std::forward<OnRead>(onRead)
            ](boost::system::error_code errorCode, std::size_t bytesTransferred) {
                onReadCopy(*thisSharedPointer, errorCode, bytesTransferred);
            }
        );
    }

    template<typename Derived, typename StreamType>
    SessionBase<Derived, StreamType>::SessionBase(StreamType &&stream) :
        // attention! moved stream
        stream_(std::move(stream))
    {}

    template<typename Derived, typename StreamType>
    StreamType & SessionBase<Derived, StreamType>::getStream() {
        return stream_;
    }
}

#endif
