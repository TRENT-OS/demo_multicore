#include "lib_debug/Debug.h"
#include "../../application_settings.h"

#include <camkes.h>
#include "OS_Dataport.h"

//------------------------------------------------------------------------------
// lock functions
#define lock_functions(_inst_) \
    void _inst_ ## _lock(void) \
    { \
        if(0 != m_lock()) \
        { \
            Debug_LOG_ERROR("Mutex was not able to be locked!"); \
        } \
    } \
    \
    void _inst_ ## _unlock(void) \
    { \
        if(0 != m_unlock()) \
        { \
            Debug_LOG_ERROR("Mutex was not able to be unlocked!"); \
        } \
    }

//------------------------------------------------------------------------------

typedef struct conf {
    uint64_t time_a;
    uint64_t time_b;
    int entered;
    int left;
    int num_readers_time_a;
    int num_readers_time_b;
    int num_readers_entered;
    int num_readers_left;
}conf_t;

OS_Dataport_t port_storage = OS_DATAPORT_ASSIGN(buffer);

/*
 * Read from a variable _var_ from a buffer buf in a thread-safe/core-safe
 * manner. We allow multiple readers in the critical region as long as no writer
 * core is accessing the critical region. As a result, we achieve mutual
 * exclusion between readers and writers. The variable ret is the data in the
 * shared data region we access.
 */
#define reader(_var_,buf,ret) \
    if(_var_ ## _read_mutex_lock()) \
    { \
        Debug_LOG_ERROR("unable to lock read mutex"); \
    } \
    buf->num_readers_ ## _var_ = buf->num_readers_ ## _var_ + 1; \
    if(buf->num_readers_ ## _var_ == 1) \
    { \
        if(_var_ ## _mutex_lock()) \
        { \
            Debug_LOG_ERROR("unable to lock resource mutex"); \
        } \
    } \
    if(_var_ ## _read_mutex_unlock()) \
    { \
        Debug_LOG_ERROR("unable to unlock reader mutex"); \
    } \
    ret = buf->_var_; \
    if(_var_ ## _read_mutex_lock()) \
    { \
        Debug_LOG_ERROR("unable to lock reader mutex"); \
    } \
    buf->num_readers_ ## _var_ = buf->num_readers_ ## _var_ - 1; \
    if(buf->num_readers_ ## _var_ == 0) \
    { \
        if(_var_ ## _mutex_unlock()) \
        { \
            Debug_LOG_ERROR("unable to unlock resource mutex"); \
        } \
    } \
    if(_var_ ## _read_mutex_unlock()) \
    { \
        Debug_LOG_ERROR("unable to unlock reader mutex"); \
    }

/*
 * We only allow one writer on the shared memory region at a time. As a result,
 * we provide a locking mechanism via resource lock. This function set the
 * variable _var_ in the shared memory region buf is set to the value val.
 */
#define writer_set(_var_,val,buf) \
    if(_var_ ## _mutex_lock()) \
    { \
        Debug_LOG_ERROR("unable to lock resource mutex"); \
    } \
    buf->_var_ = val; \
    if(_var_ ## _mutex_unlock()) \
    { \
        Debug_LOG_ERROR("unable to unlock resource mutex"); \
    }

/*
 * We only allow one writer on the shared memory region at a time. As a result,
 * we provide a locking mechanism via resource lock. This function increments
 * the variable _var_ in the shared memory region buf.
 */
#define writer_increment(_var_,buf) \
    if(_var_ ## _mutex_lock()) \
    { \
        Debug_LOG_ERROR("unable to lock resource mutex"); \
    } \
    buf->_var_++; \
    if(_var_ ## _mutex_unlock()) \
    { \
        Debug_LOG_ERROR("unable to unlock resource mutex"); \
    }

// sync functions
#define sync_functions(_inst_) \
    void _inst_ ## _set_time_a(uint64_t time) \
    { \
        conf_t * buf = (conf_t *)OS_Dataport_getBuf(port_storage); \
        writer_set(time_a,time,buf); \
    } \
    \
    void _inst_ ## _set_time_b(uint64_t time) \
    { \
        conf_t * buf = (conf_t *)OS_Dataport_getBuf(port_storage); \
        writer_set(time_b,time,buf); \
    } \
    \
    uint64_t _inst_ ## _get_time_a(void) \
    { \
        conf_t * buf = (conf_t *)OS_Dataport_getBuf(port_storage); \
        uint64_t ret = 0; \
        reader(time_a,buf,ret); \
        return ret; \
    } \
    \
    uint64_t _inst_ ## _get_time_b(void) \
    { \
        conf_t * buf = (conf_t *)OS_Dataport_getBuf(port_storage); \
        uint64_t ret = 0; \
        reader(time_b,buf,ret); \
        return ret; \
    } \
    \
    void _inst_ ## _increment_entered(void) \
    { \
        conf_t * buf = (conf_t *)OS_Dataport_getBuf(port_storage); \
        writer_increment(entered,buf); \
    } \
    \
    void _inst_ ## _increment_left(void) \
    { \
        conf_t * buf = (conf_t *)OS_Dataport_getBuf(port_storage); \
        writer_increment(left,buf); \
    } \
    \
    int _inst_ ## _get_entered(void) \
    { \
        conf_t * buf = (conf_t *)OS_Dataport_getBuf(port_storage); \
        uint64_t ret = 0; \
        reader(entered,buf,ret); \
        return ret; \
    } \
    \
    int _inst_ ## _get_left(void) \
    { \
        conf_t * buf = (conf_t *)OS_Dataport_getBuf(port_storage); \
        uint64_t ret = 0; \
        reader(left,buf,ret); \
        return ret; \
    }

void post_init(void)
{
    conf_t * buf = (conf_t *)OS_Dataport_getBuf(port_storage);
    buf->time_a = 0;
    buf->time_b = 0;
    buf->entered = 0;
    buf->left = 0;
    buf->num_readers_entered = 0;
    buf->num_readers_left = 0;
    buf->num_readers_time_a = 0;
    buf->num_readers_time_b = 0;
}

#define FUNC(A, MACRO) MACRO(A)

#if NUM_INSTANCES >= 1
FUNC(GET_LOCK(1),lock_functions)

FUNC(GET_PROC(1),sync_functions)
#endif

#if NUM_INSTANCES >= 2
FUNC(GET_LOCK(2),lock_functions)

FUNC(GET_PROC(2),sync_functions)
#endif

#if NUM_INSTANCES >= 3
FUNC(GET_LOCK(3),lock_functions)

FUNC(GET_PROC(3),sync_functions)
#endif

#if NUM_INSTANCES >= 4
FUNC(GET_LOCK(4),lock_functions)

FUNC(GET_PROC(4),sync_functions)
#endif
