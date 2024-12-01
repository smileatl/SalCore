#pragma once

#ifdef WIN32
typedef DWORD base_thread_id_t;
#else
#include <pthread.h>
typedef pthread_t base_thread_id_t;
#endif

// ��ͷ�ļ���ʹ��ǰ������
typedef struct apr_file_t base_file_t;
typedef struct apr_pool_t base_memory_pool_t;

BASE_DECLARE(base_status_t) base_apr_initialize(void);

