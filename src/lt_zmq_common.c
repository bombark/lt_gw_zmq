/* BSD 2-Clause License
 * 
 * Copyright (c) 2023, Visao Robotica Imagem (VRI)
 *   Felipe Bombardelli
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * */
	
// ============================================================================
//  Header
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <zmq.h>
#include <lt_api.h>

#include "lt_zmq_common.h"

void* g_context = NULL;

// ============================================================================
//  Topic
// ============================================================================

int lt_zmq_type(const link_t* link) {
    return 0;
}

int lt_zmq_state(const link_t* link) {
    return 0;
}

size_t lt_zmq_size(const link_t* link, int type) {
    ll_local_t* local = link->gw_shr;
    if ( local ) {
        return zmq_msg_size(&local->recv_msg);
    }
    return 0;
}

int lt_zmq_boot (link_t* link, const lt_args_t* args) {
    // get arguments
    const char* host = lt_args_gets(args, "@host", NULL);
    if ( host == NULL ) { return EINVAL; }

    const uint32_t port = lt_args_geti(args, "@port", 5000);
    void* context = (void*) lt_args_getp(args, "@context", NULL);
    if ( context == NULL ) {
        if ( g_context == NULL ) {
            g_context = zmq_ctx_new();
        }
        context = g_context;
    }

	// prepare object data with the socket parameters
	const size_t host_str_len = strlen(host);
	ll_obj_t* shr = malloc( sizeof(ll_obj_t) + host_str_len + 1 );
	shr->context = context;
	shr->port = port;
    strcpy(shr->host, host);
	link->gw_shr = shr;

    // prepare link data
    ll_local_t* local = malloc( sizeof(ll_local_t) );
    local->socket = NULL;
    zmq_msg_init(&local->recv_msg);
    link->gw_obj = local;

    // success
    return LT_OK;
}

void lt_zmq_stop(link_t* link, int type) {
    if ( type == LT_STOP_CLOSE ) {
        ll_local_t* local = link->gw_obj;
        zmq_close(local->socket);
        free(local);
        link->gw_obj = NULL;
    }
}

void lt_zmq_recv(link_t* link) {
    ll_local_t* local = link->gw_obj;
    const size_t msg_size = zmq_msg_recv (&local->recv_msg, local->socket, 0);

    local->idx = 0;

    if ( link->dec_api != NULL ) {
        uint8_t* recv_msg_data = zmq_msg_data(&local->recv_msg);
        const size_t recv_msg_size = zmq_msg_size(&local->recv_msg);
        link->dec_api->recv(link, (char*) recv_msg_data, recv_msg_size);
    }
}

bool lt_zmq_recv_async(link_t* link) {
    ll_local_t* local = link->gw_obj;
    local->idx = 0;

    const int msg_size = zmq_msg_recv (&local->recv_msg, local->socket, ZMQ_DONTWAIT);
    if ( msg_size < 0 ) {
        return false;
    }

    if ( link->dec_api != NULL ) {
        uint8_t* recv_msg_data = zmq_msg_data(&local->recv_msg);
        const size_t recv_msg_size = zmq_msg_size(&local->recv_msg);
        link->dec_api->recv(link, (char*) recv_msg_data, recv_msg_size);
    }

    return true;
}


size_t lt_zmq_read(link_t* link, char* buffer, size_t max_size) {
    ll_local_t* local = link->gw_obj;

    if ( local == NULL ) {
        return 0;
    }

    //
    const size_t offset = local->idx;
    const uint8_t* msg_data = zmq_msg_data(&local->recv_msg);
    const size_t msg_size = zmq_msg_size(&local->recv_msg);
    if ( offset > msg_size ) {
        return 0;
    }

    //
    size_t copied = 0;
    const size_t rest_of_msg = msg_size - offset;
    if ( rest_of_msg > max_size ) {
        strncpy(buffer, msg_data, max_size);
        local->idx += max_size;
        copied = max_size;
    } else {
        strncpy(buffer, msg_data, rest_of_msg);
        local->idx += rest_of_msg;
        copied = rest_of_msg;
    }

    //
    return copied;
}

size_t lt_zmq_write(link_t* link, const char* buffer, size_t size) {
    ll_local_t* local = link->gw_obj;
    if ( local == NULL ) {
        return 0;
    }

    // return zmq_send (socket, buffer, size, 0); // ZMQ_SNDMORE);
    // fprintf(stderr, "[zmq] enviado %ld bytes\n", size);
    const size_t sent = zmq_send (local->socket, buffer, size, 0);
    if ( sent != size ) {
        fprintf(stderr, "[zmq] error %ld\n", sent);
    }
    return sent;
}

// ============================================================================
//  Public Functions
// ============================================================================

const char* lt_zmq_list() {
    return "file";
}
