/**
 * @file   util_types.h
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Jan  5 16:30:48 2010
 * 
 * @brief  
 * 
 * 
 */

#ifndef __Util_TYPES_H
#define __Util_TYPES_H


typedef enum {
    RT_OK = 0x00000000, /* No error occured. */
    RT_FAILURE,         /* A general or unknown error occured. */
    RT_ERR_INIT,        /* A general initialization error occured. */
    RT_BUG,             /* Internal bug or inconsistency has been detected. */
    RT_UNSUPPORTED,     /* The requested operation or an argument is (currently) not supported. */
    RT_UNIMPLEMENTED,   /* The requested operation is not implemented, yet. */
    RT_ACCESSDENIED,    /* Access to the resource is denied. */
    RT_INVAREA,         /* An invalid area has been specified or detected. */
    RT_INVARG,          /* An invalid argument has been specified. */
    RT_NOLOCALMEMORY,   /* There's not enough local system memory. */
    RT_NOSHAREDMEMORY,  /* There's not enough shared system memory. */
    RT_LOCKED,          /* The resource is (already) locked. */
    RT_BUFFEREMPTY,     /* The buffer is empty. */
    RT_FILENOTFOUND,    /* The specified file has not been found. */
    RT_FILESEEKERROR,   /* Seek file error. */
    RT_IO,              /* A general I/O error occured. */
    RT_BUSY,            /* The resource or device is busy. */
    RT_NOTFINISHED,     /* The requested task does not been finished */
    RT_NOIMPL,          /* No implementation for this interface or content type has been found. */
    RT_TIMEOUT,         /* The operation timed out. */
    RT_IDNOTFOUND,      /* No resource has been found by the specified id. */
    RT_DESTROYED,       /* The requested object has been destroyed. */
    RT_BUFFERTOOLARGE,  /* Buffer is too large. */
    RT_INTERRUPTED,     /* The operation has been interrupted. */
    RT_NOCONTEXT,       /* No context available. */
    RT_TEMPUNAVAIL,     /* Temporarily unavailable. */
    RT_LIMITEXCEEDED,   /* Attempted to exceed limit, i.e. any kind of maximum size, count etc. */
    RT_NOSUCHMETHOD,    /* Requested method is not known. */
    RT_NOSUCHINSTANCE,  /* Requested instance is not known. */
    RT_ITEMNOTFOUND,    /* No such item found. */
    RT_VERSIONMISMATCH, /* Some versions didn't match. */
    RT_EOF,             /* Reached end of file. */
    RT_NOSUCHFILE,      /* no such file. */
    RT_LINKERROR,       /* link file error. */
    RT_UNLINKERROR,     /* unlink file error. */

    RT_NOT_USE          /* This is not a return code! must at end!!! */
} RTCode;

#define RTSuccess(c)                            \
    ((c==RT_OK) ? true : false)

#define RTCodeToStr(c)                                                  \
    (c==RT_OK)              ? " No error occured." :                    \
    (c==RT_FAILURE)         ? " A general or unknown error occured." :  \
    (c==RT_ERR_INIT)        ? " A general initialization error occured." : \
    (c==RT_BUG)             ? " Internal bug or inconsistency has been detected. " : \
    (c==RT_UNSUPPORTED)     ? " The requested operation or an argument is (currently) not supported. " : \
    (c==RT_UNIMPLEMENTED)   ? " The requested operation is not implemented) yet. " : \
    (c==RT_ACCESSDENIED)    ? " Access to the resource is denied. " :   \
    (c==RT_INVAREA)         ? " An invalid area has been specified or detected. " : \
    (c==RT_INVARG)          ? " An invalid argument has been specified. " : \
    (c==RT_NOLOCALMEMORY)   ? " There's not enough local system memory. " : \
    (c==RT_NOSHAREDMEMORY)  ? " There's not enough shared system memory. " : \
    (c==RT_LOCKED)          ? " The resource is (already) locked. " :   \
    (c==RT_BUFFEREMPTY)     ? " The buffer is empty. " :                \
    (c==RT_FILENOTFOUND)    ? " The specified file has not been found. " : \
    (c==RT_FILESEEKERROR)   ? " Seek file error. " : \
    (c==RT_IO)              ? " A general I/O error occured. " :        \
    (c==RT_BUSY)            ? " The resource or device is busy. " :     \
    (c==RT_NOIMPL)          ? " No implementation for this interface or content type has been found. " : \
    (c==RT_TIMEOUT)         ? " The operation timed out. " :            \
    (c==RT_IDNOTFOUND)      ? " No resource has been found by the specified id. " : \
    (c==RT_DESTROYED)       ? " The requested object has been destroyed. " : \
    (c==RT_BUFFERTOOLARGE)  ? " Buffer is too large. " :                \
    (c==RT_INTERRUPTED)     ? " The operation has been interrupted. " : \
    (c==RT_NOCONTEXT)       ? " No context available. " :               \
    (c==RT_TEMPUNAVAIL)     ? " Temporarily unavailable. " :            \
    (c==RT_LIMITEXCEEDED)   ? " Attempted to exceed limit, i.e. any kind of maximum size, count etc. " : \
    (c==RT_NOSUCHMETHOD)    ? " Requested method is not known. " :      \
    (c==RT_NOSUCHINSTANCE)  ? " Requested instance is not known. " :    \
    (c==RT_ITEMNOTFOUND)    ? " No such item found. " :                 \
    (c==RT_VERSIONMISMATCH) ? " Some versions didn't match. " :         \
    (c==RT_EOF)             ? " Reached end of file. " :                \
    (c==RT_NOSUCHFILE)      ? " no such file. " :                       \
    (c==RT_LINKERROR)       ? " link file error. " :                    \
    (c==RT_UNLINKERROR)     ? " unlink file error. " :                  \
    (c==RT_NOT_USE)         ? " This is not a return code! must at end!!!" : "Get unknown return code!"
    

#define SAFESTRING(x) (x==NULL) ? "" : x
#endif /// __Util_TYPES_H
