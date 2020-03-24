#include <stdio.h>
#include <stdlib.h>
#include "../ev.h"

#define HOST         "127.0.0.1"
#define PORT         5959
#define BACKLOG      128
#define STATS_PERIOD 5

static unsigned connections = 0;
static unsigned total_connections = 0;

static void print_stats(ev_context *ctx, void *data) {
    (void) ctx;  // unused
    (void) data; // unused
    printf("Connected %u total %u\n", connections, total_connections);
}

static void on_data(ev_tcp_client *client) {
    ev_tcp_read(client);
    printf("Received %li bytes\n", client->bufsize);
    if (strncmp(client->buf, "quit", 4) == 0) {
        ev_tcp_close_connection(client);
        --connections;
    } else {
        (void) ev_tcp_write(client);
    }
}

static void on_connection(ev_tcp_server *server) {
    int err = 0;
    ev_tcp_client *client = malloc(sizeof(*client));
    if ((err = ev_tcp_server_accept(server, client, on_data)) < 0) {
        if (err < 0) {
            if (err == -1)
                fprintf(stderr, "Something went wrong %s\n", strerror(errno));
            else
                fprintf(stderr, "Something went wrong %s\n", ev_tcp_err(err));
        }
    } else {
        ++connections;
        ++total_connections;
    }
}

int main(void) {

    ev_context *ctx = ev_get_ev_context();
    ev_register_cron(ctx, print_stats, NULL, STATS_PERIOD, 0);
    ev_tcp_server server;
    ev_tcp_server_init(&server, ctx, BACKLOG);
    int err = ev_tcp_server_listen(&server, HOST, PORT, on_connection);
    if (err < 0) {
        if (err == -1)
            fprintf(stderr, "Something went wrong %s\n", strerror(errno));
        else
            fprintf(stderr, "Something went wrong %s\n", ev_tcp_err(err));
    }
    ev_tcp_server_stop(&server);

    return 0;
}