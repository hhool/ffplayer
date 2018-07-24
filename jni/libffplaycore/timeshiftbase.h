#ifndef __TIMESHIFT_BASE_H__
#define __TIMESHIFT_BASE_H__

typedef struct TimeShiftInfo {
  bool      mbAllowTimeShift;
  char      mFileName[1024];
  int64_t       mFileSize;
}TimeShiftInfo;

enum media_event_timeshift_sate_type {
  TSHIFT_NOP = 0,
  TSHIFT_BEGIN_RECORD = 1,
  TSHIFT_PROGRESS = 2,
  TSHIFT_END_RECORD = 3,
  TSHIFT_OPEN_SRC_ERR = 4,
  TSHIFT_OPEN_DST_ERR = 5,
  TSHIFT_READ_SRC_ERR = 6,
  TSHIFT_WRITE_DST_ERR = 7,
  TSHIFT_PLAYER_OPEN_DST_ERR = 8
};

#endif

