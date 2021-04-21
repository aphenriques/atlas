//
//  client/sync/temporaryRedirection.hpp
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

#ifndef atlas_client_sync_temporaryRedirection_hpp
#define atlas_client_sync_temporaryRedirection_hpp

#include "Client.hpp"

namespace atlas::client::sync::temporaryRedirection {
    template<typename Connection, typename Body, typename Fields, typename OnShutdown, typename ...SslContextOrNothing>
    boost::beast::http::response<boost::beast::http::dynamic_body> request(
        boost::asio::io_context &ioContext,
        Client<Connection> &client,
        const boost::beast::http::request<Body, Fields> &requestMessage,
        int maximumNumberOfRedirections,
        const OnShutdown &onShutdown,
        SslContextOrNothing &...sslContextOrNothing
    );

    namespace detail {
        template<typename Connection, typename Body, typename Fields, typename OnShutdown, typename ...SslContextOrNothing>
        boost::beast::http::response<boost::beast::http::dynamic_body> follow(
            boost::asio::io_context &ioContext,
            Client<Connection> &client,
            const boost::beast::http::response<boost::beast::http::dynamic_body> &response,
            boost::beast::http::request<Body, Fields> requestMessage,
            int maximumNumberOfRedirections,
            const OnShutdown &onShutdown,
            SslContextOrNothing &...sslContextOrNothing
        );
    }

    //--

    template<typename Connection, typename Body, typename Fields, typename OnShutdown, typename ...SslContextOrNothing>
    boost::beast::http::response<boost::beast::http::dynamic_body> request(
        boost::asio::io_context &ioContext,
        Client<Connection> &client,
        const boost::beast::http::request<Body, Fields> &requestMessage,
        int maximumNumberOfRedirections,
        const OnShutdown &onShutdown,
        SslContextOrNothing &...sslContextOrNothing
    ) {
        boost::beast::http::response<boost::beast::http::dynamic_body> response = client.request(requestMessage);
        try {
            const boost::beast::http::status status = response.result();
            switch (status) {
                default:
                    return response;
                case boost::beast::http::status::see_other:
                    if (requestMessage.method() != boost::beast::http::verb::get) {
                        boost::beast::http::request<boost::beast::http::empty_body> newRequestMessage(requestMessage.base());
                        newRequestMessage.method(boost::beast::http::verb::get);
                        return detail::follow(
                            ioContext,
                            client,
                            response,
                            newRequestMessage,
                            maximumNumberOfRedirections,
                            onShutdown,
                            sslContextOrNothing...
                        );
                    }
                    [[fallthrough]];
                case boost::beast::http::status::found:
                    [[fallthrough]];
                case boost::beast::http::status::temporary_redirect:
                    return detail::follow(
                        ioContext,
                        client,
                        response,
                        requestMessage,
                        maximumNumberOfRedirections,
                        onShutdown,
                        sslContextOrNothing...
                    );
            }
        } catch (const std::exception &exception) {
            throw exception::RuntimeException(__FILE__, __LINE__, __func__, "request exception: [[ ", exception.what(), " ]]; request: [[ ", requestMessage, " ]]; response: [[ ", response, " ]]");
        }
    }

    namespace detail {
        template<typename Connection, typename Body, typename Fields, typename OnShutdown, typename ...SslContextOrNothing>
        boost::beast::http::response<boost::beast::http::dynamic_body> follow(
            boost::asio::io_context &ioContext,
            Client<Connection> &client,
            const boost::beast::http::response<boost::beast::http::dynamic_body> &response,
            boost::beast::http::request<Body, Fields> requestMessage,
            int maximumNumberOfRedirections,
            const OnShutdown &onShutdown,
            SslContextOrNothing &...sslContextOrNothing
        ) {
            if (maximumNumberOfRedirections > 0) {
                const auto locationFieldIterator = response.find(boost::beast::http::field::location);
                if (locationFieldIterator != response.cend()) {
                    const boost::beast::string_view newLocation = locationFieldIterator->value();
                    if (newLocation.empty() == false) {
                        if (newLocation[0] != '/') {
                            requestMessage.target(newLocation);
                            return request(
                                ioContext,
                                client,
                                requestMessage,
                                maximumNumberOfRedirections - 1,
                                onShutdown,
                                sslContextOrNothing...
                            );
                        } else {
                            Url newUrl{std::string(newLocation)};
                            requestMessage.set(boost::beast::http::field::host, newUrl.getHost());
                            requestMessage.target(newUrl.getTarget());
                            Client<Connection> temporaryClient(ioContext, newUrl, sslContextOrNothing...);
                            const boost::beast::http::response<boost::beast::http::dynamic_body> newResponse = request(
                                ioContext,
                                temporaryClient,
                                requestMessage,
                                maximumNumberOfRedirections - 1,
                                onShutdown,
                                sslContextOrNothing...
                            );
                            boost::system::error_code errorCode;
                            temporaryClient.shutdown(errorCode);
                            onShutdown(errorCode);
                            return newResponse;
                        }
                    } else {
                        throw exception::RuntimeException(__FILE__, __LINE__, __func__, "empty location");
                    }
                } else {
                    throw exception::RuntimeException(__FILE__, __LINE__, __func__, "no location field");
                }
            } else {
                throw exception::RuntimeException(__FILE__, __LINE__, __func__, "maximum number of redirections reached");
            }
        }
    }
}

#endif
