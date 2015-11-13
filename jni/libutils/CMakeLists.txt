#
# Util specific library fuctions 
#

LIST(APPEND	libutils_SRC
  util_log.cpp
  util_lock.cpp
  util_thread.cpp
  util_time.cpp
  util_timer.cpp
  util_uri.cpp
  util_network.cpp
  util_file.cpp
  util_fsinfo.cpp
  util_folder.cpp
  util_eventloop.cpp
  util_md5sum.cpp
  util_crc32.cpp
  util_format.cpp
  util_ringbuffer.cpp
  util_argument.cpp
  util_cmdline.cpp
  )

LIST(APPEND libutils_HEADERS
  util_log.h
  util_lock.h
  util_thread.h
  util_time.h
  util_timer.h
  util_uri.h
  util_network.h
  util_file.h
  util_fsinfo.h
  util_folder.h
  util_eventloop.h
  util_md5sum.h
  util_crc32.h
  util_format.h
  util_ringbuffer.h
  util_argument.h
  util_cmdline.h
)

ADD_LIBRARY(utils ${DS_LIBRARY_TYPE} ${libutils_SRC})
TARGET_LINK_LIBRARIES(utils pthread)

ADD_SUBDIRECTORY(test)

