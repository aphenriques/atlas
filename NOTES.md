# TODO
- timeouts
- report uneeded make_strand on libs/beast/example/http/server/async/http_server_async.cpp:373 (https://www.boost.org/doc/libs/1_75_0/libs/beast/example/http/server/async/http_server_async.cpp)

# boost::beast

## Expected errors

https://www.boost.org/doc/libs/1_75_0/libs/beast/example/http/server/async-ssl/http_server_async_ssl.cpp:

```
ssl::error::stream_truncated, also known as an SSL "short read", indicates the peer closed the connection without performing the required closing handshake (for example, Google does this to improve performance). Generally this can be a security issue, but if your communication protocol is self-terminated (as it is with both HTTP and WebSocket) then you may simply ignore the lack of close_notify.
https://github.com/boostorg/beast/issues/38
https://security.stackexchange.com/questions/91435/how-to-handle-a-malicious-ssl-tls-shutdown
When a short read would cut off the end of an HTTP message, Beast returns the error beast::http::error::partial_message.  Therefore, if we see a short read here, it has occurred after the message has been completed, so it is safe to ignore it.
```

http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error

## boost::asio::ssl::context thread safety

https://stackoverflow.com/a/33519766:

```
Because SSL contexts needs to be shared among SSL sessions (otherwise, how would session resumption work) the SSL context class is fully, internally thread safe. You can use an SSL context in multiple SSL connections and from multiple threads however you want.
```

## No strand on http server

https://stackoverflow.com/questions/12794107/why-do-i-need-strand-per-connection-when-using-boostasio

https://www.boost.org/doc/libs/1_75_0/doc/html/boost_asio/overview/core/strands.html:

```
Where there is a single chain of asynchronous operations associated with a connection (e.g. in a half duplex protocol implementation like HTTP) there is no possibility of concurrent execution of the handlers. This is an implicit strand.
```
