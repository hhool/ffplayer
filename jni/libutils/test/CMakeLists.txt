INCLUDE_DIRECTORIES(
  ../
  )


##fsinfo
ADD_EXECUTABLE(test_fsinfo  test_fsinfo.cpp)
TARGET_LINK_LIBRARIES (test_fsinfo  utils)
##crc32
ADD_EXECUTABLE(test_crc32  test_crc32.cpp)
TARGET_LINK_LIBRARIES (test_crc32  utils)
##md5
ADD_EXECUTABLE(test_md5  test_md5.cpp)
TARGET_LINK_LIBRARIES (test_md5  utils)
##timer
ADD_EXECUTABLE(test_timer  test_timer.cpp)
TARGET_LINK_LIBRARIES (test_timer  utils)
