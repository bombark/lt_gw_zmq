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

#include <lt_api.h>

extern void* g_context;

typedef struct {
    uint16_t port;
    void* context;
    char host[];
} ll_obj_t;

typedef struct {
    void* socket;
    zmq_msg_t recv_msg;
    size_t idx;
} ll_local_t;

// ============================================================================
//  Common Functions
// ============================================================================

int lt_zmq_type(const link_t* link);
int lt_zmq_state(const link_t* link);
size_t lt_zmq_size(const link_t* link, int type);
int lt_zmq_boot (link_t* link, const lt_args_t* args);
void lt_zmq_stop(link_t* link, int type);
void lt_zmq_recv(link_t* link);
bool lt_zmq_recv_async(link_t* link);
size_t lt_zmq_read(link_t* link, char* buffer, size_t max_size);
size_t lt_zmq_write(link_t* link, const char* buffer, size_t size);

// ============================================================================
//  Public Functions
// ============================================================================

const char* lt_zmq_list();
