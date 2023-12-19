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

// ============================================================================
//  Socket
// ============================================================================

static
int lt_zmq_socket_start(struct _link* link, int type, const lt_args_t* args) {

    if ( type == LT_START_CONNECT ) {
        //
        const ll_obj_t* obj_args = link->gw_obj;
        void* socket = zmq_socket (obj_args->context, ZMQ_REQ);
        if ( socket == NULL ) {
            return 1;
        }

        // 
        char url[512];
        snprintf(url, sizeof(url), "tcp://%s:%d", obj_args->host, obj_args->port);
fprintf(stderr, "client %s\n", url);
        if ( zmq_connect (socket, url) != 0 ) {
            return 1;
        }

        //
        ll_local_t* local = link->gw_obj;
        local->socket = socket;

    } else if ( type == LT_START_BIND ) {
        //
        const ll_obj_t* shr_data = link->gw_shr;
        void* socket = zmq_socket (shr_data->context, ZMQ_REP);
        if ( socket == NULL ) {
            return 1;
        }

        //
        char url[512];
        snprintf(url, sizeof(url), "tcp://%s:%d", shr_data->host, shr_data->port);
fprintf(stderr, "server %s\n", url);
        int error = zmq_bind (socket, url);
        if ( error != 0 ) {
            printf("%d\n", error);
            return 1;
        }

        //
        ll_local_t* local = link->gw_obj;
        local->socket = socket;
    }

    return 0;
}

static
lt_api_t lt_zmq_socket = {
	.type = lt_zmq_type,
	.state = lt_zmq_state,
	.size = lt_zmq_size,
	.boot = lt_zmq_boot,
	.start = lt_zmq_socket_start,
	.stop = lt_zmq_stop,
	.copy = NULL,
    .recv = lt_zmq_recv,
    .recv_async = lt_zmq_recv_async,
	.read = lt_zmq_read,
	.write = lt_zmq_write,
};

int lt_new_zmq_socket(link_t* link, const lt_args_t* args) {
	link->gw_api = &lt_zmq_socket;
	lt_zmq_boot(link, args);
	return LT_OK;
}
