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

/* MIDIMuteSolo implementation */

#include "sles_allinclusive.h"


static SLresult IMIDIMuteSolo_SetChannelMute(SLMIDIMuteSoloItf self, SLuint8 channel,
    SLboolean mute)
{
    SL_ENTER_INTERFACE

    if (channel > 15) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        IMIDIMuteSolo *this = (IMIDIMuteSolo *) self;
        SLuint16 mask = 1 << channel;
        interface_lock_exclusive(this);
        if (mute)
            this->mChannelMuteMask |= mask;
        else
            this->mChannelMuteMask &= ~mask;
        interface_unlock_exclusive(this);
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IMIDIMuteSolo_GetChannelMute(SLMIDIMuteSoloItf self, SLuint8 channel,
    SLboolean *pMute)
{
    SL_ENTER_INTERFACE

    if (channel > 15 || (NULL == pMute)) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        IMIDIMuteSolo *this = (IMIDIMuteSolo *) self;
        interface_lock_peek(this);
        SLuint16 mask = this->mChannelMuteMask;
        interface_unlock_peek(this);
        *pMute = (mask >> channel) & 1;
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IMIDIMuteSolo_SetChannelSolo(SLMIDIMuteSoloItf self, SLuint8 channel,
    SLboolean solo)
{
    SL_ENTER_INTERFACE

    if (channel > 15) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        IMIDIMuteSolo *this = (IMIDIMuteSolo *) self;
        SLuint16 mask = 1 << channel;
        interface_lock_exclusive(this);
        if (solo)
            this->mChannelSoloMask |= mask;
        else
            this->mChannelSoloMask &= ~mask;
        interface_unlock_exclusive(this);
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IMIDIMuteSolo_GetChannelSolo(SLMIDIMuteSoloItf self, SLuint8 channel,
    SLboolean *pSolo)
{
    SL_ENTER_INTERFACE

    if (channel > 15 || (NULL == pSolo)) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        IMIDIMuteSolo *this = (IMIDIMuteSolo *) self;
        interface_lock_peek(this);
        SLuint16 mask = this->mChannelSoloMask;
        interface_unlock_peek(this);
        *pSolo = (mask >> channel) & 1;
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IMIDIMuteSolo_GetTrackCount(SLMIDIMuteSoloItf self, SLuint16 *pCount)
{
    SL_ENTER_INTERFACE

    if (NULL == pCount) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        IMIDIMuteSolo *this = (IMIDIMuteSolo *) self;
        // const, so no lock needed
        SLuint16 trackCount = this->mTrackCount;
        *pCount = trackCount;
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IMIDIMuteSolo_SetTrackMute(SLMIDIMuteSoloItf self, SLuint16 track, SLboolean mute)
{
    SL_ENTER_INTERFACE

    IMIDIMuteSolo *this = (IMIDIMuteSolo *) self;
    // const
    if (!(track < this->mTrackCount)) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        SLuint32 mask = 1 << track;
        interface_lock_exclusive(this);
        if (mute)
            this->mTrackMuteMask |= mask;
        else
            this->mTrackMuteMask &= ~mask;
        interface_unlock_exclusive(this);
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IMIDIMuteSolo_GetTrackMute(SLMIDIMuteSoloItf self, SLuint16 track, SLboolean *pMute)
{
    SL_ENTER_INTERFACE

    IMIDIMuteSolo *this = (IMIDIMuteSolo *) self;
    // const, no lock needed
    if (!(track < this->mTrackCount) || NULL == pMute) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        interface_lock_peek(this);
        SLuint32 mask = this->mTrackMuteMask;
        interface_unlock_peek(this);
        *pMute = (mask >> track) & 1;
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IMIDIMuteSolo_SetTrackSolo(SLMIDIMuteSoloItf self, SLuint16 track, SLboolean solo)
{
    SL_ENTER_INTERFACE

    IMIDIMuteSolo *this = (IMIDIMuteSolo *) self;
    // const
    if (!(track < this->mTrackCount)) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        SLuint32 mask = 1 << track; interface_lock_exclusive(this);
        if (solo)
            this->mTrackSoloMask |= mask;
        else
            this->mTrackSoloMask &= ~mask;
        interface_unlock_exclusive(this);
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IMIDIMuteSolo_GetTrackSolo(SLMIDIMuteSoloItf self, SLuint16 track, SLboolean *pSolo)
{
    SL_ENTER_INTERFACE

    IMIDIMuteSolo *this = (IMIDIMuteSolo *) self;
    // const, no lock needed
    if (!(track < this->mTrackCount) || NULL == pSolo) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        interface_lock_peek(this);
        SLuint32 mask = this->mTrackSoloMask;
        interface_unlock_peek(this);
        *pSolo = (mask >> track) & 1;
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static const struct SLMIDIMuteSoloItf_ IMIDIMuteSolo_Itf = {
    IMIDIMuteSolo_SetChannelMute,
    IMIDIMuteSolo_GetChannelMute,
    IMIDIMuteSolo_SetChannelSolo,
    IMIDIMuteSolo_GetChannelSolo,
    IMIDIMuteSolo_GetTrackCount,
    IMIDIMuteSolo_SetTrackMute,
    IMIDIMuteSolo_GetTrackMute,
    IMIDIMuteSolo_SetTrackSolo,
    IMIDIMuteSolo_GetTrackSolo
};

void IMIDIMuteSolo_init(void *self)
{
    IMIDIMuteSolo *this = (IMIDIMuteSolo *) self;
    this->mItf = &IMIDIMuteSolo_Itf;
    this->mChannelMuteMask = 0;
    this->mChannelSoloMask = 0;
    this->mTrackMuteMask = 0;
    this->mTrackSoloMask = 0;
    // const
    this->mTrackCount = 32; // wrong
}
