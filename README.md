EV
==

Light event-loop library loosely inspired by the excellent libuv, in a single
**small** (< 700 sloc) header, based on the common IO multiplexing
implementations available, epoll on linux, kqueue on BSD-like and OSX,
poll/select as a fallback, dependencies-free.
A common usage of the library is to craft event-driven TCP servers, `ev_tcp.h`
exposes a set of APIs to fulfill this purpose in a simple manner.

TLS is supported as well through OpenSSL, and source have to be compiled adding
a `-DHAVE_OPENSSL=1` to enable it.

## Running examples

A simple event-driven echo server

```
$ make echo-server
```

Write periodically on the screen `ping` and `pong` on different frequencies,
referred as cron tasks

```
$ make ping-pong
```

### Helper APIs

Lightweight event-driven hello world TCP server

```c
#include <stdio.h>
#include <stdlib.h>
#include "../ev_tcp.h"

static void on_data(ev_tcp_handle *client) {
    printf("Received %li bytes\n", client->buffer.size);
    if (strncmp(client->buffer.buf, "quit", 4) == 0)
        ev_tcp_close_connection(client);
    else
        // If using TLS encryption
        // ev_tls_tcp_write(client);
        ev_tcp_write(client);
}

static void on_connection(ev_tcp_handle *server) {
    ev_tcp_handle *client = malloc(sizeof(*client));
    iev_tcp_server_accept(server, client, on_data);
}

int main(void) {

    ev_context *ctx = ev_get_ev_context();
    ev_tcp_server server;
    ev_tcp_server_init(&server, ctx, 128);
    // To set TLS using OpenSSL
    // struct ev_tls_options tls_opt = {
    //     .ca = CA,
    //     .cert = CERT,
    //     .key = KEY
    // };
    // tls_opt.protocols = EV_TLSv1_2|EV_TLSv1_3;
    // ev_tcp_server_set_tls(&server, &tls_opt);
    ev_tcp_server_listen(&server, "127.0.0.1", 5959, on_connection);
    // Blocking call
    ev_tcp_server_run(&server);
    // This could be registered to a SIGINT|SIGTERM signal notification
    // to stop the server with Ctrl+C
    ev_tcp_server_stop(&server);

    return 0;
}
```

Take a look to `examples/` directory for more snippets.

## Roadmap

- (Re)Move server abstraction on generic `ev_tcp_handle`, add client
- UDP helper APIs
- TLS setup
- Improve error handling
- Documentation
