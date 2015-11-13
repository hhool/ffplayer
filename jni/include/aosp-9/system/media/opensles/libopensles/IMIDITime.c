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

/* MIDITime implementation */

#include "sles_allinclusive.h"


static SLresult IMIDITime_GetDuration(SLMIDITimeItf self, SLuint32 *pDuration)
{
    SL_ENTER_INTERFACE

    if (NULL == pDuration) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        IMIDITime *this = (IMIDITime *) self;
        SLuint32 duration = this->mDuration;
        *pDuration = duration;
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IMIDITime_SetPosition(SLMIDITimeItf self, SLuint32 position)
{
    SL_ENTER_INTERFACE

    IMIDITime *this = (IMIDITime *) self;
    // const, no lock needed
    if (!(position < this->mDuration)) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        interface_lock_poke(this);
        this->mPosition = position;
        interface_unlock_poke(this);
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IMIDITime_GetPosition(SLMIDITimeItf self, SLuint32 *pPosition)
{
    SL_ENTER_INTERFACE

    if (NULL == pPosition) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        IMIDITime *this = (IMIDITime *) self;
        interface_lock_peek(this);
        SLuint32 position = this->mPosition;
        interface_unlock_peek(this);
        *pPosition = position;
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IMIDITime_SetLoopPoints(SLMIDITimeItf self, SLuint32 startTick, SLuint32 numTicks)
{
    SL_ENTER_INTERFACE

    IMIDITime *this = (IMIDITime *) self;
    // const, no lock needed
    SLuint32 duration = this->mDuration;
    if (!((startTick < duration) && (numTicks <= duration - startTick))) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        interface_lock_exclusive(this);
        this->mStartTick = startTick;
        this->mNumTicks = numTicks;
        interface_unlock_exclusive(this);
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IMIDITime_GetLoopPoints(SLMIDITimeItf self, SLuint32 *pStartTick,
    SLuint32 *pNumTicks)
{
    SL_ENTER_INTERFACE

    if (NULL == pStartTick || NULL == pNumTicks) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        IMIDITime *this = (IMIDITime *) self;
        interface_lock_shared(this);
        SLuint32 startTick = this->mStartTick;
        SLuint32 numTicks = this->mNumTicks;
        interface_unlock_shared(this);
        *pStartTick = startTick;
        *pNumTicks = numTicks;
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static const struct SLMIDITimeItf_ IMIDITime_Itf = {
    IMIDITime_GetDuration,
    IMIDITime_SetPosition,
    IMIDITime_GetPosition,
    IMIDITime_SetLoopPoints,
    IMIDITime_GetLoopPoints
};

void IMIDITime_init(void *self)
{
    IMIDITime *this = (IMIDITime *) self;
    this->mItf = &IMIDITime_Itf;
    this->mDuration = 0;
    this->mPosition = 0;
    this->mStartTick = 0;
    this->mNumTicks = 0;
}
