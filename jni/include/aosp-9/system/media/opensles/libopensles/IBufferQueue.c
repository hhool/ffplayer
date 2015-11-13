/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* BufferQueue implementation */

#include "sles_allinclusive.h"


/** Determine the state of the audio player or audio recorder associated with a buffer queue.
 *  Note that PLAYSTATE and RECORDSTATE values are equivalent (where PLAYING == RECORDING).
 */

static SLuint32 getAssociatedState(IBufferQueue *this)
{
    SLuint32 state;
    switch (InterfaceToObjectID(this)) {
    case SL_OBJECTID_AUDIOPLAYER:
        state = ((CAudioPlayer *) this->mThis)->mPlay.mState;
        break;
    case SL_OBJECTID_AUDIORECORDER:
        state = ((CAudioRecorder *) this->mThis)->mRecord.mState;
        break;
    default:
        // unreachable, but just in case we will assume it is stopped
        assert(SL_BOOLEAN_FALSE);
        state = SL_PLAYSTATE_STOPPED;
        break;
    }
    return state;
}


SLresult IBufferQueue_Enqueue(SLBufferQueueItf self, const void *pBuffer, SLuint32 size)
{
    SL_ENTER_INTERFACE
    //SL_LOGV("IBufferQueue_Enqueue(%p, %p, %lu)", self, pBuffer, size);

    // Note that Enqueue while a Clear is pending is equivalent to Enqueue followed by Clear

    if (NULL == pBuffer || 0 == size) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        IBufferQueue *this = (IBufferQueue *) self;
        interface_lock_exclusive(this);
        BufferHeader *oldRear = this->mRear, *newRear;
        if ((newRear = oldRear + 1) == &this->mArray[this->mNumBuffers + 1]) {
            newRear = this->mArray;
        }
        if (newRear == this->mFront) {
            result = SL_RESULT_BUFFER_INSUFFICIENT;
        } else {
            oldRear->mBuffer = pBuffer;
            oldRear->mSize = size;
            this->mRear = newRear;
            ++this->mState.count;
            result = SL_RESULT_SUCCESS;
        }
        // set enqueue attribute if state is PLAYING and the first buffer is enqueued
        interface_unlock_exclusive_attributes(this, ((SL_RESULT_SUCCESS == result) &&
            (1 == this->mState.count) && (SL_PLAYSTATE_PLAYING == getAssociatedState(this))) ?
            ATTR_ENQUEUE : ATTR_NONE);
    }
    SL_LEAVE_INTERFACE
}


SLresult IBufferQueue_Clear(SLBufferQueueItf self)
{
    SL_ENTER_INTERFACE

    result = SL_RESULT_SUCCESS;
    IBufferQueue *this = (IBufferQueue *) self;
    interface_lock_exclusive(this);

#ifdef ANDROID
    if (SL_OBJECTID_AUDIOPLAYER == InterfaceToObjectID(this)) {
        CAudioPlayer *audioPlayer = (CAudioPlayer *) this->mThis;
        // flush associated audio player
        result = android_audioPlayer_bufferQueue_onClear(audioPlayer);
        if (SL_RESULT_SUCCESS == result) {
            this->mFront = &this->mArray[0];
            this->mRear = &this->mArray[0];
            this->mState.count = 0;
            this->mState.playIndex = 0;
            this->mSizeConsumed = 0;
        }
    }
#endif

#ifdef USE_OUTPUTMIXEXT
    // mixer might be reading from the front buffer, so tread carefully here
    // NTH asynchronous cancel instead of blocking until mixer acknowledges
    this->mClearRequested = SL_BOOLEAN_TRUE;
    do {
        interface_cond_wait(this);
    } while (this->mClearRequested);
#endif

    interface_unlock_exclusive(this);

    SL_LEAVE_INTERFACE
}


static SLresult IBufferQueue_GetState(SLBufferQueueItf self, SLBufferQueueState *pState)
{
    SL_ENTER_INTERFACE

    // Note that GetState while a Clear is pending is equivalent to GetState before the Clear

    if (NULL == pState) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        IBufferQueue *this = (IBufferQueue *) self;
        SLBufferQueueState state;
        interface_lock_shared(this);
#ifdef __cplusplus // FIXME Is this a compiler bug?
        state.count = this->mState.count;
        state.playIndex = this->mState.playIndex;
#else
        state = this->mState;
#endif
        interface_unlock_shared(this);
        *pState = state;
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


SLresult IBufferQueue_RegisterCallback(SLBufferQueueItf self,
    slBufferQueueCallback callback, void *pContext)
{
    SL_ENTER_INTERFACE

    IBufferQueue *this = (IBufferQueue *) self;
    interface_lock_exclusive(this);
    // verify pre-condition that media object is in the SL_PLAYSTATE_STOPPED state
    if (SL_PLAYSTATE_STOPPED == getAssociatedState(this)) {
        this->mCallback = callback;
        this->mContext = pContext;
        result = SL_RESULT_SUCCESS;
    } else {
        result = SL_RESULT_PRECONDITIONS_VIOLATED;
    }
    interface_unlock_exclusive(this);

    SL_LEAVE_INTERFACE
}


static const struct SLBufferQueueItf_ IBufferQueue_Itf = {
    IBufferQueue_Enqueue,
    IBufferQueue_Clear,
    IBufferQueue_GetState,
    IBufferQueue_RegisterCallback
};

void IBufferQueue_init(void *self)
{
    IBufferQueue *this = (IBufferQueue *) self;
    this->mItf = &IBufferQueue_Itf;
    this->mState.count = 0;
    this->mState.playIndex = 0;
    this->mCallback = NULL;
    this->mContext = NULL;
    this->mNumBuffers = 0;
    this->mClearRequested = SL_BOOLEAN_FALSE;
    this->mArray = NULL;
    this->mFront = NULL;
    this->mRear = NULL;
#ifdef ANDROID
    this->mSizeConsumed = 0;
#endif
    BufferHeader *bufferHeader = this->mTypical;
    unsigned i;
    for (i = 0; i < BUFFER_HEADER_TYPICAL+1; ++i, ++bufferHeader) {
        bufferHeader->mBuffer = NULL;
        bufferHeader->mSize = 0;
    }
}


/** \brief Interface deinitialization hook called by IObject::Destroy.
 *  Free the buffer queue, if it was larger than typical.
 */

void IBufferQueue_deinit(void *self)
{
    IBufferQueue *this = (IBufferQueue *) self;
    if ((NULL != this->mArray) && (this->mArray != this->mTypical)) {
        free(this->mArray);
        this->mArray = NULL;
    }
}
