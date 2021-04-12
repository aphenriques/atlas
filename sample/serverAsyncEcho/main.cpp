//
//  serverAsyncEcho/main.cpp
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
#include <atomic>
#include <thread>
#include <atlas/server/async/Server.hpp>
#include "server_certificate.hpp"
#include "handler.hpp"

// testing:
// curl -v http://localhost -d 'ok!'
// curl -v -k https://localhost -d 'ok!'

int main() {
    try {
        boost::asio::io_context ioContext;
        atlas::server::async::Server server(
            ioContext,
            boost::asio::ip::tcp::v4(),
            80,
            handler::OnAccept(&handler::startSession)
        );
        boost::asio::ssl::context sslContext(boost::asio::ssl::context::tlsv12);
        load_server_certificate(sslContext);
        atlas::server::async::Server secureServer(
            ioContext,
            boost::asio::ip::tcp::v4(),
            443,
            handler::OnAccept(handler::SecureSessionStarter(sslContext))
        );
        std::atomic_bool successfulRun = true;
        auto runIoContext = [&ioContext, &successfulRun] {
            try {
                ioContext.run();
            } catch (const std::exception &exception) {
                ioContext.stop();
                successfulRun = false;
                std::lock_guard<std::mutex> lockGuard(handler::global::outputMutex);
                std::cerr << "ioContext.run() error: " << exception.what() << std::endl;
            }
        };
        std::thread thread(runIoContext);
        runIoContext();
        thread.join();
        return successfulRun == true ? EXIT_SUCCESS : EXIT_FAILURE;
    } catch (const std::exception &exception) {
        std::cerr << "Error: " << exception.what() << std::endl;
        return EXIT_FAILURE;
    }
}
