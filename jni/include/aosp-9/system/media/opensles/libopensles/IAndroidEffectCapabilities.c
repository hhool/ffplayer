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

/* Android Effect Capabilities implementation */

#include "sles_allinclusive.h"


static SLresult IAndroidEffectCapabilities_QueryNumEffects(SLAndroidEffectCapabilitiesItf self,
        SLuint32 * pNumSupportedAudioEffects) {

    SL_ENTER_INTERFACE

    if (NULL == pNumSupportedAudioEffects) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        IAndroidEffectCapabilities *this = (IAndroidEffectCapabilities *) self;
        interface_lock_peek(this);

        *pNumSupportedAudioEffects = this->mNumFx;
        result = SL_RESULT_SUCCESS

        interface_unlock_peek(this);
    }

    SL_LEAVE_INTERFACE
}


static SLresult IAndroidEffectCapabilities_QueryEffect(SLAndroidEffectCapabilitiesItf self,
        SLuint32 index, SLInterfaceID *pEffectType, SLInterfaceID *pEffectImplementation,
        SLchar * pName, SLuint16 *pNameSize) {

    SL_ENTER_INTERFACE

    IAndroidEffectCapabilities *this = (IAndroidEffectCapabilities *) self;
    if (index > this->mNumFx) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        interface_lock_peek(this);
        if (NULL != pEffectType) {
            *pEffectType = (SLInterfaceID) &this->mFxDescriptors[index].type;
        }
        if (NULL != pEffectImplementation) {
            *pEffectImplementation = (SLInterfaceID) &this->mFxDescriptors[index].uuid;
        }
        if ((NULL != pName) && (0 < *pNameSize)) {
            int len = strlen(this->mFxDescriptors[index].name);
            strncpy((char*)pName, this->mFxDescriptors[index].name,
                    *pNameSize > len ? len : *pNameSize );
            *pNameSize = len;
        }
        interface_unlock_peek(this);
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static const struct SLAndroidEffectCapabilitiesItf_ IAndroidEffectCapabilities_Itf = {
        IAndroidEffectCapabilities_QueryNumEffects,
        IAndroidEffectCapabilities_QueryEffect
};

void IAndroidEffectCapabilities_init(void *self)
{
    IAndroidEffectCapabilities *this = (IAndroidEffectCapabilities *) self;
    this->mItf = &IAndroidEffectCapabilities_Itf;

    // This is the default initialization; fields will be updated when interface is exposed
    this->mNumFx = 0;
    this->mFxDescriptors = NULL;
}

bool IAndroidEffectCapabilities_Expose(void *self)
{
    IAndroidEffectCapabilities *this = (IAndroidEffectCapabilities *) self;
    SLuint32 numEffects = 0;
    SLresult result = android_genericFx_queryNumEffects(&numEffects);
    if (SL_RESULT_SUCCESS != result) {
        SL_LOGE("android_genericFx_queryNumEffects %lu", result);
        return false;
    }
    this->mNumFx = numEffects;
    SL_LOGV("Effect Capabilities has %ld effects", this->mNumFx);
    if (this->mNumFx > 0) {
        this->mFxDescriptors = (effect_descriptor_t*) new effect_descriptor_t[this->mNumFx];
        for (SLuint32 i = 0 ; i < this->mNumFx ; i++) {
            SLresult result2;
            result2 = android_genericFx_queryEffect(i, &this->mFxDescriptors[i]);
            if (SL_RESULT_SUCCESS != result2) {
                SL_LOGE("Error (SLresult is %ld) querying effect %ld", result2, i);
                // Remember the first failing result code, but keep going
                if (SL_RESULT_SUCCESS == result) {
                    result = result2;
                }
            } else {
                SL_LOGV("effect %ld: type=%08x-%04x-%04x-%04x-%02x%02x%02x%02x%02x%02x name=%s",
                        i,
                        this->mFxDescriptors[i].type.timeLow,
                        this->mFxDescriptors[i].type.timeMid,
                        this->mFxDescriptors[i].type.timeHiAndVersion,
                        this->mFxDescriptors[i].type.clockSeq,
                        this->mFxDescriptors[i].type.node[0],
                        this->mFxDescriptors[i].type.node[1],
                        this->mFxDescriptors[i].type.node[2],
                        this->mFxDescriptors[i].type.node[3],
                        this->mFxDescriptors[i].type.node[4],
                        this->mFxDescriptors[i].type.node[5],
                        this->mFxDescriptors[i].name);
            }
        }
    }
    return SL_RESULT_SUCCESS == result;
}

void IAndroidEffectCapabilities_deinit(void *self)
{
    IAndroidEffectCapabilities *this = (IAndroidEffectCapabilities *) self;
    // free effect library data
    if (NULL != this->mFxDescriptors) {
        delete[] this->mFxDescriptors;
        this->mFxDescriptors = NULL;
    }
}
