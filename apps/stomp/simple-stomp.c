#include "simple-stomp.h"

#include "contiki.h"
#include "contiki-net.h"

#include "stomp-tools.h"
#include "stomp-frame.h"
#include "stomp-strings.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ISO_NULL 0x00
#define ISO_NL 0x0a
#define ISO_COLON 0x3a

const char version[4] = {0x31, 0x2e, 0x31,};

const char default_content_type[11] = {0x74, 0x65, 0x78, 0x74, 0x2f, 0x70, 0x6c, 0x61, 0x69, 0x6e,};

static
PT_THREAD(simple_send(struct simple_stomp_state *s)) {
    s->outputbuf_len = stomp_frame_length(s->frame);
    stomp_frame_export(s->frame, (char*) s->outputbuf, s->outputbuf_len);

    stomp_frame_delete_frame(s->frame);
    s->frame = NULL;

    PSOCK_BEGIN(&s->s);
    printf("%s\n", s->outputbuf);
    PSOCK_SEND(&s->s, (uint8_t*) s->outputbuf, (unsigned int) s->outputbuf_len);
    PSOCK_END(&s->s);
}

static
PT_THREAD(simple_handle_connection(struct simple_stomp_state *s)) {
    PT_BEGIN(&s->pt);
    printf("simple_handle_connection: start.\n");

    PSOCK_INIT(&s->s, (uint8_t*) s->inputbuf, sizeof (s->inputbuf) - 1);
    PSOCK_WAIT_UNTIL(&s->s, PSOCK_NEWDATA(&s->s) || (s->frame != NULL));

    if (PSOCK_NEWDATA(&s->s)) {
        PSOCK_READTO(&s->s, ISO_NULL);
        if (PSOCK_DATALEN(&s->s) > 0) {
            s->inputbuf[PSOCK_DATALEN(&s->s)] = 0;
            printf("%s\n", s->inputbuf);
            /* TODO import frame */
        }
    }
    if (s->frame != NULL) {
        s->outputbuf_len = stomp_frame_length(s->frame);
        stomp_frame_export(s->frame, (char*) s->outputbuf, s->outputbuf_len);

        stomp_frame_delete_frame(s->frame);
        s->frame = NULL;

        printf("%s\n", s->outputbuf);
        PSOCK_SEND(&s->s, (uint8_t*) s->outputbuf, (unsigned int) s->outputbuf_len);
    }

    printf("simple_handle_connection: stop.\n");
    PT_END(&s->pt);
}

void
simple_app(void *s) {
    struct simple_stomp_state *state = (struct simple_stomp_state*) s;

    printf("simple_app: start.\n");

    if (uip_aborted() || uip_timedout() || uip_closed()) {
        simple_disconnected(s);

    } else if (uip_connected()) {
        simple_connected(s);
        PT_INIT(&state->pt);
        simple_handle_connection(s);

    } else if (s != NULL) {
        simple_handle_connection(s);
    }

    printf("simple_app: stop.\n");
}

struct simple_stomp_state *
simple_connect(struct simple_stomp_state *s, uip_ipaddr_t *ipaddr, uint16_t port, char *host, char *login, char *pass) {
    printf("simple_connect: start.\n");

    s->conn = tcp_connect(ipaddr, UIP_HTONS(port), s);
    printf("Connecting...\n");

    if (s->conn == NULL) {
        printf("Not connected.\n");
        return NULL;
    }

    s->host = host;
    s->login = login;
    s->pass = pass;

    printf("simple_connect: stop.\n");

    return s;
}

void
simple_connected(struct simple_stomp_state *s) {
    struct stomp_header *headers = NULL;

    printf("simple_connected: start.\n");

    headers = stomp_frame_new_header(stomp_header_accept_version, version);
    if (s->pass != NULL) {
        headers = stomp_frame_add_header(stomp_header_passcode, s->pass, headers);
    }
    if (s->login != NULL) {
        headers = stomp_frame_add_header(stomp_header_login, s->login, headers);
    }
    if (s->host != NULL) {
        headers = stomp_frame_add_header(stomp_header_host, s->host, headers);
    } else {
        printf("No host for CONNECT. Abort.\n");
        stomp_frame_delete_header(headers);
        return;
    }

    s->frame = stomp_frame_new_frame(stomp_command_connect, headers, NULL);

    simple_send(s);

    printf("simple_connected: stop.\n");
}

void
simple_stomp_subscribe(struct simple_stomp_state *s, char *id, char *destination, char *ack) {
    struct stomp_header *headers = NULL;

    printf("simple_stomp_subscribe: start.\n");

    if (ack != NULL) {
        headers = stomp_frame_new_header(stomp_header_ack, ack);
    } else {
        printf("No ack for SUBSCRIBE. Set to 'auto'.\n");
        headers = stomp_frame_new_header(stomp_header_ack, stomp_header_auto);
    }
    if (destination != NULL) {
        headers = stomp_frame_add_header(stomp_header_destination, destination, headers);
    } else {
        printf("No destination for SUBSCRIBE. Abort.\n");
        stomp_frame_delete_header(headers);
        return;
    }
    if (id != NULL) {
        headers = stomp_frame_add_header(stomp_header_id, id, headers);
    } else {
        printf("No id for SUBSCRIBE. Abort.\n");
        stomp_frame_delete_header(headers);
        return;
    }

    s->frame = stomp_frame_new_frame(stomp_command_subscribe, headers, NULL);

    printf("simple_stomp_subscribe: stop.\n");
}

void
simple_stomp_unsubscribe(struct simple_stomp_state *s, char *id) {
    struct stomp_header *headers = NULL;

    printf("simple_stomp_unsubscribe: start.\n");

    if (id != NULL) {
        headers = stomp_frame_new_header(stomp_header_id, id);
    } else {
        printf("No id for UNSUBSCRIBE. Abort.\n");
        stomp_frame_delete_header(headers);
        return;
    }

    s->frame = stomp_frame_new_frame(stomp_command_unsubscribe, headers, NULL);

    printf("simple_stomp_unsubscribe: stop.\n");
}

void
simple_stomp_send(struct simple_stomp_state *s, char *destination, char *type, char *length, char *receipt, char *tx, char *message) {
    struct stomp_header *headers = NULL;

    printf("simple_stomp_send: start.\n");

    if (tx != NULL) {
        headers = stomp_frame_new_header(stomp_header_transaction, tx);
    }
    if (receipt != NULL) {
        headers = stomp_frame_add_header(stomp_header_receipt, receipt, headers);
    }
    if (length != NULL) {
        headers = stomp_frame_add_header(stomp_header_content_length, length, headers);
    } else {
        printf("No content-length for SEND. Set to computed value.\n");
        length = NEW_ARRAY(char, 3);
        sprintf(length, "%u", (unsigned int) strlen(message));
        headers = stomp_frame_add_header(stomp_header_content_length, length, headers);
    }
    if (type != NULL) {
        headers = stomp_frame_add_header(stomp_header_content_type, type, headers);
    } else {
        printf("No content-type for SEND. Set to 'text/plain'.\n");
        headers = stomp_frame_add_header(stomp_header_content_type, default_content_type, headers);
    }
    if (destination != NULL) {
        headers = stomp_frame_add_header(stomp_header_destination, destination, headers);
    } else {
        printf("No destination for SEND. Abort.\n");
        stomp_frame_delete_header(headers);
        return;
    }

    s->frame = stomp_frame_new_frame(stomp_command_send, headers, message);

    printf("simple_stomp_send: stop.\n");
}

void
simple_stomp_begin(struct simple_stomp_state *s, char *tx) {
    struct stomp_header *headers = NULL;

    printf("simple_stomp_begin: start.\n");

    if (tx != NULL) {
        headers = stomp_frame_new_header(stomp_header_transaction, tx);
    } else {
        printf("No tx for BEGIN. Abort.\n");
        stomp_frame_delete_header(headers);
        return;
    }

    s->frame = stomp_frame_new_frame(stomp_command_begin, headers, NULL);

    printf("simple_stomp_begin: stop.\n");
}

void
simple_stomp_commit(struct simple_stomp_state *s, char *tx) {
    struct stomp_header *headers = NULL;

    printf("simple_stomp_commit: start.\n");

    if (tx != NULL) {
        headers = stomp_frame_new_header(stomp_header_transaction, tx);
    } else {
        printf("No tx for COMMIT. Abort.\n");
        stomp_frame_delete_header(headers);
        return;
    }

    s->frame = stomp_frame_new_frame(stomp_command_commit, headers, NULL);

    printf("simple_stomp_commit: stop.\n");
}

void
simple_stomp_abort(struct simple_stomp_state *s, char *tx) {
    struct stomp_header *headers = NULL;

    printf("simple_stomp_abort: start.\n");

    if (tx != NULL) {
        headers = stomp_frame_new_header(stomp_header_transaction, tx);
    } else {
        printf("No tx for ABORT. Abort.\n");
        stomp_frame_delete_header(headers);
        return;
    }

    s->frame = stomp_frame_new_frame(stomp_command_abort, headers, NULL);

    printf("simple_stomp_abort: stop.\n");
}

void
simple_stomp_disconnect(struct simple_stomp_state *s, char *receipt) {
    struct stomp_header *headers = NULL;

    printf("simple_stomp_disconnect: start.\n");

    if (receipt != NULL) {
        headers = stomp_frame_new_header(stomp_header_receipt, receipt);
    }

    s->frame = stomp_frame_new_frame(stomp_command_disconnect, headers, NULL);

    printf("simple_stomp_disconnect: stop.\n");
}

void
simple_disconnected(struct simple_stomp_state *s) {
    printf("Disconnected.\n");
}