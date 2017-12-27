#ifndef __TRANSPORT_ICE_H__
#define __TRANSPORT_ICE_H__

#include <stdint.h>
#include <sys/time.h>
#include <pthread.h>

#ifdef __APPLE__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdocumentation"
#endif

#include <pjlib.h>
#include <pjnath.h>

#ifdef __APPLE__
#pragma GCC diagnostic pop
#endif

#include "session.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct IceWorker {
    TransportWorker  base;

    pj_bool_t           regular;

    int                 quit;

    pj_str_t            stun_server;
    int                 stun_port;
    pj_str_t            turn_server;
    int                 turn_port;
    pj_str_t            turn_username;
    pj_str_t            turn_password;
    pj_bool_t           turn_fingerprint;

    pj_caching_pool     cp;
    pj_ice_strans_cfg   cfg;
    pj_pool_t           *pool;
    pj_thread_t         *thread;

    pj_sockaddr_in      read_addr;
    pj_ioqueue_key_t    *read_key;
    pj_ioqueue_key_t    *write_key;
} IceWorker;

typedef struct IceTransport {
    ElaTransport        base;
    pthread_key_t       pj_thread_ctx;
} IceTransport;

typedef struct IceSession {
    ElaSession          base;

    pj_ice_sess_role    role;
    char                ufrag[PJ_ICE_UFRAG_LEN+1];
    char                pwd[PJ_ICE_UFRAG_LEN+1];
} IceSession;

typedef struct IceStream {
    ElaStream           base;
    StreamHandler       *handler;

    struct timeval      local_timestamp;
    struct timeval      remote_timestamp;
    Timer               *keepalive_timer;
} IceStream;

typedef struct IceHandler {
    StreamHandler       base;

    pj_ice_strans       *st;

    int                 stopping;
        
    struct {
        char            ufrag[80];
        char            pwd[80];
        unsigned int    comp_cnt;
        pj_sockaddr     def_addr[PJ_ICE_MAX_COMP];
        unsigned int    cand_cnt;
        pj_ice_sess_cand    cand[PJ_ICE_ST_MAX_CAND];
    } remote;
} IceHandler;

int ice_transport_create(ElaTransport **transport);

#ifdef __cplusplus
}
#endif

#endif /* __TRANSPORT_ICE_H__ */
