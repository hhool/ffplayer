/**
 * @file   util_ringbuffer.cpp
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Jan 12 18:10:29 2010
 * 
 * @brief  
 * 
 * 
 */
#include "util_ringbuffer.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>

/*
 * Ring buffer io functions.
 *
 */

UtilRingBuffer::UtilRingBuffer(unsigned char *_buf, int _buf_size):
    m_pbuf (_buf),
    m_size (_buf_size),
    m_balloc (false),
    m_data_needed(0),
    m_free_needed(0),
    mb_pre_buffering(false),
    m_pre_buffer_size(-1)
{
    if (_buf_size <= 0) {
        fprintf (stderr, "Fatal Error! init buffer size == %d\n", _buf_size);
    }
        
    if (NULL == m_pbuf) {
        //fprintf (stderr, "Alloc %lld memory for ringbuffer!\n", m_size);
        m_pbuf = (unsigned char*) new char[m_size];
        if (m_pbuf == NULL) {
            fprintf (stderr, "Fatal Error! new memory error! m_pbuf == NULL!\n");
        }
        m_balloc = true;
    }
    
    m_pdata = m_pempty = m_pbuf;
    *m_pempty = 0;
    
    pthread_cond_init (&m_data_cond, NULL);
    pthread_cond_init (&m_free_cond, NULL);
    pthread_cond_init (&m_pre_buffer_cond, NULL);

    pthread_mutex_init (&m_lock, NULL);
}

UtilRingBuffer::~UtilRingBuffer() {
    flush ();

    pthread_cond_destroy (&m_free_cond);
    pthread_cond_destroy (&m_data_cond);
    pthread_cond_destroy (&m_pre_buffer_cond);

    pthread_mutex_destroy (&m_lock);
    
    if (m_balloc) {
        //fprintf (stderr, "Delete the memory allocated for the ringbuffer!\n");
        delete[] m_pbuf;
    }
}

unsigned int UtilRingBuffer::getFilePointer() {
    unsigned int ret = 0;

    pthread_mutex_lock (&m_lock);
    ret += (unsigned int)m_pempty;
    pthread_mutex_unlock (&m_lock);

    return ret;
}


// In order not to add another interface, We use the flush function to reset the ringbuffer
// 
void UtilRingBuffer::flush () {
    pthread_mutex_lock (&m_lock);

    m_pdata = m_pempty = m_pbuf;
    *m_pempty = 0;
    memset (m_pbuf, 0, m_size);
    if (m_free_needed)
        pthread_cond_broadcast (&m_free_cond);

    if (m_data_needed)
        pthread_cond_broadcast (&m_data_cond);

    pthread_mutex_unlock (&m_lock);
}


int UtilRingBuffer::pre_buffer (int size) {
    pthread_mutex_lock (&m_lock);
    struct timeval now;
    struct timespec ts;
    int data_size = 0;

    if (m_pdata <= m_pempty) {
        data_size = m_pempty - m_pdata;
    } else {
        data_size = m_pempty + m_size - m_pdata;
    }

    m_pre_buffer_size = size;

    fprintf (stderr, "%s %d: Prebuffering..., data_size=%d, pre buffer size=%d, total=%d!\n",
             __FUNCTION__, __LINE__, data_size, m_pre_buffer_size, m_size);    
    
    if (data_size < m_pre_buffer_size) {
        gettimeofday (&now, NULL);
        ts.tv_sec = now.tv_sec;
        ts.tv_nsec = now.tv_usec*1000;

        ts.tv_sec += 10;
        mb_pre_buffering = true;

        fprintf (stderr, "%s %d: Prebuffering..., pre buffer size=%d, total=%d!\n", 
                 __FUNCTION__, __LINE__, m_pre_buffer_size, m_size);    
        pthread_cond_timedwait (&m_pre_buffer_cond, &m_lock, &ts);
        fprintf (stderr, "%s %d: Prebuffer finished, pre buffer size = %d!\n", 
                 __FUNCTION__, __LINE__, m_pre_buffer_size);    
    
        mb_pre_buffering = false;
        m_pre_buffer_size = -1;
    }

    if (m_pdata <= m_pempty) {
        data_size = m_pempty - m_pdata;
    } else {
        data_size = m_pempty + m_size - m_pdata;
    }

    pthread_mutex_unlock (&m_lock);
    return data_size;
}

int UtilRingBuffer::pre_buffer (float percent) {
    int size = (int) (m_size * percent);
    return pre_buffer (size);
}

int UtilRingBuffer::readBlocking (void *buffer, int size) {
    int data_size, data_size1, data_size2;
    int ret = 0;
    unsigned char *buf = (unsigned char *)buffer;

    //assert((buffer != NULL) && (size > 0));
    if (NULL == buffer || size <= 0) {
        fprintf (stderr,"Invalid buffer or size to read from ringbuffer!\n");
        return 0;
    }

    // Lock
    pthread_mutex_lock (&m_lock);

    //monitor ();
    
    do {
        if (m_pdata <= m_pempty) {
            data_size = m_pempty - m_pdata;
            data_size1 = data_size;
            data_size2 = 0;
        } else {
            data_size = m_pempty + m_size - m_pdata;
            data_size2 = m_pempty - m_pbuf;
            data_size1 = data_size - data_size2;
        }

        // We need more data to read
        if (size > data_size) {
            m_data_needed++;
            //fprintf (stderr, "We need more data to read! size = %d, data_size = %d\n", size, data_size);
            //monitor ();
            pthread_cond_wait (&m_data_cond, &m_lock);
            //fprintf (stderr, "New data available, recheck...!\n");
            m_data_needed--;
            continue;
        }
        
        // Yeah, we have got enough data to read
        break;
    } while (1);

    if (size > data_size1) {
        memcpy (buf, m_pdata, data_size1);
        m_pdata += data_size1;
        if (m_pdata >= (m_pbuf + m_size))
            m_pdata -= m_size;
        ret += data_size1;
    } else {
        memcpy (buf, m_pdata, size);
        m_pdata += size;
        if (m_pdata >= (m_pbuf + m_size))
            m_pdata -= m_size;
        
        // HAHA we have read the data and let's go home
        pthread_mutex_unlock (&m_lock);

        // Notify new free available
        if (m_free_needed)
            pthread_cond_broadcast (&m_free_cond);

        return size;
    }

    if (!data_size2) {
        pthread_mutex_unlock (&m_lock);

        // Notify new free available
        if (m_free_needed)
            pthread_cond_broadcast (&m_free_cond);
        
        return ret;
    }

    // Got other part of data
    if ((size - ret) <= data_size2) {
        memcpy (buf+data_size1, m_pbuf, (size - ret));
        m_pdata += (size - ret);
        ret += (size - ret);

        // HAHA we have read the data and let's go home
        pthread_mutex_unlock (&m_lock);

        // Notify new free available
        if (m_free_needed)
            pthread_cond_broadcast (&m_free_cond);

        return ret;
    }

    // Something wrong occured, we got nothing :(
    pthread_mutex_unlock (&m_lock);
    fprintf (stderr, "ringbuffer: read wrong!\n");
    return 0;
}

int UtilRingBuffer::readNonBlocking(void* buffer, int size) {
    int data_size, data_size1, data_size2;
    int ret = 0;
    unsigned char *buf = (unsigned char *)buffer;

    //assert((buffer != NULL) && (size > 0));
    if (NULL == buffer || size <= 0) {
        fprintf (stderr,"Invalid buffer or size to read from ringbuffer!\n");
        return 0;
    }

    // Lock
    pthread_mutex_lock (&m_lock);

    //monitor ();
    
    do {
        if (m_pdata <= m_pempty) {
            data_size = m_pempty - m_pdata;
            data_size1 = data_size;
            data_size2 = 0;
        } else {
            data_size = m_pempty + m_size - m_pdata;
            data_size2 = m_pempty - m_pbuf;
            data_size1 = data_size - data_size2;
        }

        // We need more data to read, not to wait, return current datasize.
        if (size > data_size) {
            size = data_size;
            continue;
        }
        
        // Yeah, we have got enough data to read
        break;
    } while (1);

    if (size > data_size1) {
        memcpy (buf, m_pdata, data_size1);
        m_pdata += data_size1;
        if (m_pdata >= (m_pbuf + m_size))
            m_pdata -= m_size;
        ret += data_size1;
    } else {
        memcpy (buf, m_pdata, size);
        m_pdata += size;
        if (m_pdata >= (m_pbuf + m_size))
            m_pdata -= m_size;
        
        // HAHA we have read the data and let's go home
        pthread_mutex_unlock (&m_lock);

        // Notify new free available
        if (m_free_needed)
            pthread_cond_broadcast (&m_free_cond);

        return size;
    }

    if (!data_size2) {
        pthread_mutex_unlock (&m_lock);

        // Notify new free available
        if (m_free_needed)
            pthread_cond_broadcast (&m_free_cond);
        
        return ret;
    }

    // Got other part of data
    if ((size - ret) <= data_size2) {
        memcpy (buf+data_size1, m_pbuf, (size - ret));
        m_pdata += (size - ret);
        ret += (size - ret);

        // HAHA we have read the data and let's go home
        pthread_mutex_unlock (&m_lock);

        // Notify new free available
        if (m_free_needed)
            pthread_cond_broadcast (&m_free_cond);

        return ret;
    }

    // Something wrong occured, we got nothing :(
    pthread_mutex_unlock (&m_lock);
    fprintf (stderr, "ringbuffer: read wrong!\n");
    return 0;
}


int UtilRingBuffer::write (const void *buffer, int size) {
    int empty_size, empty_size1, empty_size2;
    int ret = 0;

    unsigned char *buf = (unsigned char *)buffer;

    //assert((buffer != NULL) && (size > 0));
    if (NULL == buffer || size <= 0) {
        fprintf (stderr,"Invalid buffer or size to write to ringbuffer!\n");
        return 0;
    }
    
    // Lock
    pthread_mutex_lock (&m_lock);
    //monitor ();

    do {
        if (m_pdata <= m_pempty) {
            empty_size = m_size - (m_pempty - m_pdata) - 1;
            empty_size2 = (m_pdata == m_pbuf) ? 0 : (m_pdata - m_pbuf - 1);
            empty_size1 = empty_size - empty_size2;
        } else {
            empty_size = (m_pdata - m_pempty) - 1;
            empty_size1 = empty_size;
            empty_size2 = 0;
        }
        
        //DEBUG ("empty_size = %d, empty_size1 = %d, empty_size2 = %d", empty_size, empty_size1, empty_size2);
        if (mb_pre_buffering && (m_size - empty_size) >= m_pre_buffer_size) {
            
            // Pre buffer finished, release the lock and notify demuxer
            pthread_mutex_unlock (&m_lock);

            // Notify more data now available
            pthread_cond_broadcast (&m_pre_buffer_cond);
            
            // Lock again, continue our work
            pthread_mutex_lock (&m_lock);
        }

        // We need more free space to write
        if (empty_size < size) {
            m_free_needed++;
            //DEBUG ("We need more %ld bytes(total %d) space to write, now waiting...\n", size - empty_size, m_size);
            //monitor ();

            // Before waiting, release the prebuffer cond
            if (mb_pre_buffering) {
            
                // Pre buffer finished, release the lock and notify demuxer
                pthread_mutex_unlock (&m_lock);

                // Notify more data now available
                pthread_cond_broadcast (&m_pre_buffer_cond);
            
                // Lock again, continue our work
                pthread_mutex_lock (&m_lock);
            }

            pthread_cond_wait (&m_free_cond, &m_lock);
            //fprintf (stderr, "Free space available, recheck...\n");
            m_free_needed--;
            continue;
        } else {
            //DEBUG ("Ready to Write: empty_size = %d, write_size = %d, buf = %p", empty_size, size, buf);
            break;
        }
    } while (1);

    if (size > empty_size1) {
        memcpy (m_pempty, buf, empty_size1);
        m_pempty += empty_size1;
        if (m_pempty >= (m_pbuf + m_size))
            m_pempty -= m_size;
        ret += empty_size1;
    } else {
        memcpy (m_pempty, buf, size);
        m_pempty += size;
        if (m_pempty >= m_pbuf + m_size)
            m_pempty -= m_size;
        *(m_pempty) = 0;

        // HAHA, we have done the writing and let's go home
        pthread_mutex_unlock (&m_lock);

        // Notify more data now available
        if (m_data_needed)
            pthread_cond_broadcast (&m_data_cond);

        return size;
    }

    if (!empty_size2) {
        *(m_pempty) = 0;

        // HAHA, we have done the writing and let's go home
        pthread_mutex_unlock (&m_lock);

        // Notify more data now available
        if (m_data_needed)
            pthread_cond_broadcast (&m_data_cond);
        
        return ret;
    }

    if ((size - ret) <= empty_size2) {
        memcpy (m_pbuf, buf+empty_size1, (size - ret));
        m_pempty += (size - ret);
        *(m_pempty) = 0;
        ret += (size - ret);

        // HAHA, we have done the writing and let's go home
        pthread_mutex_unlock (&m_lock);
    
        // Notify more data now available
        if (m_data_needed)
            pthread_cond_broadcast (&m_data_cond);

        return ret;
    }

    // Hmm, something wrong happened
    pthread_mutex_unlock (&m_lock);
    fprintf (stderr, "ringbuffer: write wrong!\n");
    return 0;
}

bool UtilRingBuffer::empty () {
        return true;
}

int UtilRingBuffer::searchchr (char c) {
    return 0;
}

int UtilRingBuffer::size () {
    return m_size;
}

int UtilRingBuffer::cache_size () {
    int data_size = 0;
    pthread_mutex_lock (&m_lock);

    if (m_pdata <= m_pempty) {
        data_size = m_pempty - m_pdata;
    } else {
        data_size = m_pempty + m_size - m_pdata;
    }

    pthread_mutex_unlock (&m_lock);
    return data_size;
}

void UtilRingBuffer::clear () {

}

int UtilRingBuffer::fromfile (int fd, int size) {
	int re = 0;
	while (size) {
		int tc = size;
		int r = read(fd, m_pbuf, tc);
		if (r < 0) {
			if (errno == EINTR) continue;
			if (errno != EWOULDBLOCK)
				fprintf (stderr, "couldn't read: %m\n");
			r = 0;
		}

		size -= r;
		re += r;

		if (r != tc)
			break;
	}
    
	return re;
}
    
int UtilRingBuffer::tofile (int fd, int size) {
    return -1;
}

void UtilRingBuffer::monitor () {
	fprintf (stderr, "[buffer_ptr       %s         %s   ]\n", 
             (m_pdata > m_pempty)?"empt_ptr":"data_ptr", (m_pdata > m_pempty)?"data_ptr":"empt_ptr");
	fprintf (stderr, "[0x%p     0x%p     0x%p]\n", 
             m_pbuf, (m_pdata > m_pempty)? m_pempty:m_pdata, (m_pdata > m_pempty)? m_pdata:m_pempty);
}

