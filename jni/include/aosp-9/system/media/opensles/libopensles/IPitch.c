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

/* Pitch implementation */

#include "sles_allinclusive.h"


static SLresult IPitch_SetPitch(SLPitchItf self, SLpermille pitch)
{
    SL_ENTER_INTERFACE

    IPitch *this = (IPitch *) self;
    // const, so no lock needed
    if (!(this->mMinPitch <= pitch && pitch <= this->mMaxPitch)) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        interface_lock_poke(this);
        this->mPitch = pitch;
        interface_unlock_poke(this);
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IPitch_GetPitch(SLPitchItf self, SLpermille *pPitch)
{
    SL_ENTER_INTERFACE

    if (NULL == pPitch) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        IPitch *this = (IPitch *) self;
        interface_lock_peek(this);
        SLpermille pitch = this->mPitch;
        interface_unlock_peek(this);
        *pPitch = pitch;
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IPitch_GetPitchCapabilities(SLPitchItf self,
    SLpermille *pMinPitch, SLpermille *pMaxPitch)
{
    SL_ENTER_INTERFACE

    // per spec, each is optional, and does not require that at least one must be non-NULL
#if 0
    if (NULL == pMinPitch && NULL == pMaxPitch)
        result = SL_RESULT_PARAMETER_INVALID;
#endif
    IPitch *this = (IPitch *) self;
    // const, so no lock needed
    SLpermille minPitch = this->mMinPitch;
    SLpermille maxPitch = this->mMaxPitch;
    if (NULL != pMinPitch)
        *pMinPitch = minPitch;
    if (NULL != pMaxPitch)
        *pMaxPitch = maxPitch;
    result = SL_RESULT_SUCCESS;

    SL_LEAVE_INTERFACE
}


static const struct SLPitchItf_ IPitch_Itf = {
    IPitch_SetPitch,
    IPitch_GetPitch,
    IPitch_GetPitchCapabilities
};

void IPitch_init(void *self)
{
    IPitch *this = (IPitch *) self;
    this->mItf = &IPitch_Itf;
    this->mPitch = 1000;
    // const
    this->mMinPitch = -500;
    this->mMaxPitch = 2000;
}
