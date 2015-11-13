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

/* Vibra implementation */

#include "sles_allinclusive.h"


static SLresult IVibra_Vibrate(SLVibraItf self, SLboolean vibrate)
{
    SL_ENTER_INTERFACE

    IVibra *this = (IVibra *) self;
    interface_lock_poke(this);
    this->mVibrate = SL_BOOLEAN_FALSE != vibrate; // normalize
    interface_unlock_poke(this);
    result = SL_RESULT_SUCCESS;

    SL_LEAVE_INTERFACE
}


static SLresult IVibra_IsVibrating(SLVibraItf self, SLboolean *pVibrating)
{
    SL_ENTER_INTERFACE

    if (NULL == pVibrating) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        IVibra *this = (IVibra *) self;
        interface_lock_peek(this);
        SLboolean vibrate = this->mVibrate;
        interface_unlock_peek(this);
        *pVibrating = vibrate;
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IVibra_SetFrequency(SLVibraItf self, SLmilliHertz frequency)
{
    SL_ENTER_INTERFACE

    const SLVibraDescriptor *d = Vibra_id_descriptors[0].descriptor;
    if (!d->supportsFrequency) {
        result = SL_RESULT_PRECONDITIONS_VIOLATED;
    } else if (!(d->minFrequency <= frequency && frequency <= d->maxFrequency)) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        IVibra *this = (IVibra *) self;
        interface_lock_poke(this);
        this->mFrequency = frequency;
        interface_unlock_exclusive(this);
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IVibra_GetFrequency(SLVibraItf self, SLmilliHertz *pFrequency)
{
    SL_ENTER_INTERFACE

    if (NULL == pFrequency) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        IVibra *this = (IVibra *) self;
        interface_lock_peek(this);
        SLmilliHertz frequency = this->mFrequency;
        interface_unlock_peek(this);
        *pFrequency = frequency;
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IVibra_SetIntensity(SLVibraItf self, SLpermille intensity)
{
    SL_ENTER_INTERFACE

    const SLVibraDescriptor *d = Vibra_id_descriptors[0].descriptor;
    if (!d->supportsIntensity) {
        result = SL_RESULT_PRECONDITIONS_VIOLATED;
    } else if (!(0 <= intensity && intensity <= 1000)) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        IVibra *this = (IVibra *) self;
        interface_lock_poke(this);
        this->mIntensity = intensity;
        interface_unlock_poke(this);
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IVibra_GetIntensity(SLVibraItf self, SLpermille *pIntensity)
{
    SL_ENTER_INTERFACE

    if (NULL == pIntensity) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        const SLVibraDescriptor *d = Vibra_id_descriptors[0].descriptor;
        if (!d->supportsIntensity) {
            result = SL_RESULT_PRECONDITIONS_VIOLATED;
        } else {
            IVibra *this = (IVibra *) self;
            interface_lock_peek(this);
            SLpermille intensity = this->mIntensity;
            interface_unlock_peek(this);
            *pIntensity = intensity;
            result = SL_RESULT_SUCCESS;
        }
    }

    SL_LEAVE_INTERFACE
}


static const struct SLVibraItf_ IVibra_Itf = {
    IVibra_Vibrate,
    IVibra_IsVibrating,
    IVibra_SetFrequency,
    IVibra_GetFrequency,
    IVibra_SetIntensity,
    IVibra_GetIntensity
};

void IVibra_init(void *self)
{
    IVibra *this = (IVibra *) self;
    this->mItf = &IVibra_Itf;
    this->mVibrate = SL_BOOLEAN_FALSE;
    // next 2 values are undefined per spec
    this->mFrequency = Vibra_id_descriptors[0].descriptor->minFrequency;
    this->mIntensity = 1000;
}
