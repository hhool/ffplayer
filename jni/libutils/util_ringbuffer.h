/**
 * @file   util_ringbuffer.h
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Jan 12 18:10:52 2010
 * 
 * @brief  
 * 
 * 
 */
#ifndef __UTIL_RINGBUFFER_H__
#define __UTIL_RINGBUFFER_H__

#include <pthread.h>
#include "config/stl_config.h"

class UtilRingBuffer {
protected:
    unsigned char *m_pbuf;
    unsigned char *m_pdata;
    unsigned char *m_pempty;
    int m_size;

    bool m_balloc;

    pthread_mutex_t m_lock;
    pthread_cond_t  m_free_cond;
    pthread_cond_t  m_data_cond;

    int         m_data_needed;
    int         m_free_needed;

    bool            mb_pre_buffering;
    int             m_pre_buffer_size;
    pthread_cond_t  m_pre_buffer_cond;

private:
    void monitor (void);

public:
    UtilRingBuffer(unsigned char *_buf, int _buf_size);
    virtual ~UtilRingBuffer();

    virtual unsigned int getFilePointer();

    virtual int readBlocking(void *buffer, int size);

	virtual int readNonBlocking(void* buffer, int size);

    virtual int write(const void *buffer, int size);

    virtual void close(){}

    virtual bool eof(){return false;}

    virtual unsigned char *get_and_lock_buffer(){return m_pbuf;}

    virtual void flush ();

    virtual int pre_buffer (int size);
    virtual int pre_buffer (float percent);

    virtual string get_file_name() const {
        return "";
    }

    virtual bool empty ();

    virtual int searchchr (char c);

    virtual int size ();

    virtual int cache_size ();

    virtual void clear ();

    virtual int fromfile (int fd, int size);
    
    virtual int tofile (int fd, int size);
};

#endif
