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

/* AndroidEffectSend implementation */

#include "sles_allinclusive.h"


static SLresult IAndroidEffectSend_EnableEffectSend(SLAndroidEffectSendItf self,
    SLInterfaceID effectImplementationId, SLboolean enable, SLmillibel initialLevel)
{
    SL_ENTER_INTERFACE

    if (!((SL_MILLIBEL_MIN <= initialLevel) && (initialLevel <= 0))) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        IAndroidEffectSend *this = (IAndroidEffectSend *) self;
        interface_lock_exclusive(this);
        // is SLAndroidEffectSendItf on an AudioPlayer?
        CAudioPlayer *ap = (SL_OBJECTID_AUDIOPLAYER == InterfaceToObjectID(this)) ?
                (CAudioPlayer *) this->mThis : NULL;
        if (NULL == ap) {
            SL_LOGE("invalid interface: not attached to an AudioPlayer");
            result = SL_RESULT_PARAMETER_INVALID;
        } else {
            COutputMix *outputMix = CAudioPlayer_GetOutputMix(ap);
#ifdef USE_BACKPORT
            result = SL_RESULT_SUCCESS;
#else
            // the initial send level set here is the total energy on the aux bus,
            //  so it must take into account the player volume level
            result = android_fxSend_attachToAux(ap, effectImplementationId, enable,
                    initialLevel + ap->mVolume.mLevel);
#endif
            if (SL_RESULT_SUCCESS == result) {
                // there currently is support for only one send bus, so there is a single send
                // level and a single enable flag
                this->mSendLevel = initialLevel;
                this->mEnabled = enable;
            }
        }
        interface_unlock_exclusive(this);
    }

    SL_LEAVE_INTERFACE
}


static SLresult IAndroidEffectSend_IsEnabled(SLAndroidEffectSendItf self,
    SLInterfaceID effectImplementationId, SLboolean *pEnable)
{
    SL_ENTER_INTERFACE

    if (NULL == pEnable) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        IAndroidEffectSend *this = (IAndroidEffectSend *) self;
        interface_lock_shared(this);
        // there currently is support for only one send bus, so there is a single enable flag
        SLboolean enable = this->mEnabled;
        interface_unlock_shared(this);
        *pEnable = enable;
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IAndroidEffectSend_SetDirectLevel(SLAndroidEffectSendItf self,
        SLmillibel directLevel)
{
    SL_ENTER_INTERFACE

     if (!((SL_MILLIBEL_MIN <= directLevel) && (directLevel <= 0))) {
         result = SL_RESULT_PARAMETER_INVALID;
     } else {
         IAndroidEffectSend *this = (IAndroidEffectSend *) self;
         interface_lock_exclusive(this);
         CAudioPlayer *ap = (SL_OBJECTID_AUDIOPLAYER == InterfaceToObjectID(this)) ?
                 (CAudioPlayer *) this->mThis : NULL;
         if (NULL != ap) {
             SLmillibel oldDirectLevel = ap->mDirectLevel;
             if (oldDirectLevel != directLevel) {
                 ap->mDirectLevel = directLevel;
                 ap->mAmplFromDirectLevel = sles_to_android_amplification(directLevel);
                 interface_unlock_exclusive_attributes(this, ATTR_GAIN);
             } else {
                 interface_unlock_exclusive(this);
             }
             result = SL_RESULT_SUCCESS;
         } else {
             interface_unlock_exclusive(this);
             SL_LOGE("invalid interface: not attached to an AudioPlayer");
             result = SL_RESULT_PARAMETER_INVALID;
         }
     }

     SL_LEAVE_INTERFACE
}


static SLresult IAndroidEffectSend_GetDirectLevel(SLAndroidEffectSendItf self,
        SLmillibel *pDirectLevel)
{
    SL_ENTER_INTERFACE

    if (NULL == pDirectLevel) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        IAndroidEffectSend *this = (IAndroidEffectSend *) self;
        interface_lock_peek(this);
        CAudioPlayer *ap = (SL_OBJECTID_AUDIOPLAYER == InterfaceToObjectID(this)) ?
                (CAudioPlayer *) this->mThis : NULL;
        if (NULL != ap) {
            *pDirectLevel = ap->mDirectLevel;
            result = SL_RESULT_SUCCESS;
        } else {
            SL_LOGE("invalid interface: not attached to an AudioPlayer");
            result = SL_RESULT_PARAMETER_INVALID;
        }
        interface_unlock_peek(this);
    }

    SL_LEAVE_INTERFACE
}


static SLresult IAndroidEffectSend_SetSendLevel(SLAndroidEffectSendItf self,
        SLInterfaceID effectImplementationId, SLmillibel sendLevel)
{
    SL_ENTER_INTERFACE

    if (!((SL_MILLIBEL_MIN <= sendLevel) && (sendLevel <= 0))) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        IAndroidEffectSend *this = (IAndroidEffectSend *) self;
        interface_lock_exclusive(this);
        // is SLAndroidEffectSendItf on an AudioPlayer?
        CAudioPlayer *ap = (SL_OBJECTID_AUDIOPLAYER == InterfaceToObjectID(this)) ?
                 (CAudioPlayer *) this->mThis : NULL;
        if (NULL == ap) {
            SL_LOGE("invalid interface: not attached to an AudioPlayer");
            result = SL_RESULT_PARAMETER_INVALID;
        } else {
            COutputMix *outputMix = CAudioPlayer_GetOutputMix(ap);
#ifdef USE_BACKPORT
            result = SL_RESULT_SUCCESS;
#else
            if (android_genericFx_hasEffect(&outputMix->mAndroidEffect, effectImplementationId)) {
                // the send level set here is the total energy on the aux bus, so it must take
                // into account the player volume level
                result = android_fxSend_setSendLevel(ap, sendLevel + ap->mVolume.mLevel);
            } else {
                 SL_LOGE("trying to send to an effect not on this AudioPlayer's OutputMix");
                 result = SL_RESULT_PARAMETER_INVALID;
            }
#endif
            if (SL_RESULT_SUCCESS == result) {
                // there currently is support for only one send bus, so there is a single send
                // level
                this->mSendLevel = sendLevel;
            }
        }
        interface_unlock_exclusive(this);
    }

    SL_LEAVE_INTERFACE
}


static SLresult IAndroidEffectSend_GetSendLevel(SLAndroidEffectSendItf self,
        SLInterfaceID effectImplementationId, SLmillibel *pSendLevel)
{
    SL_ENTER_INTERFACE

    if (NULL == pSendLevel) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        IAndroidEffectSend *this = (IAndroidEffectSend *) self;
        interface_lock_exclusive(this);
        // is SLAndroidEffectSendItf on an AudioPlayer?
        CAudioPlayer *ap = (SL_OBJECTID_AUDIOPLAYER == InterfaceToObjectID(this)) ?
                (CAudioPlayer *) this->mThis : NULL;
        if (NULL == ap) {
            SL_LOGE("invalid interface: not attached to an AudioPlayer");
            result = SL_RESULT_PARAMETER_INVALID;
        } else {
            COutputMix *outputMix = CAudioPlayer_GetOutputMix(ap);
#ifdef USE_BACKPORT
            result = SL_RESULT_SUCCESS;
#else
            if (android_genericFx_hasEffect(&outputMix->mAndroidEffect, effectImplementationId)) {
                result = SL_RESULT_SUCCESS;
            } else {
                SL_LOGE("trying to retrieve send level on an effect not on this AudioPlayer's \
OutputMix");
                result = SL_RESULT_PARAMETER_INVALID;
                }
#endif
            if (SL_RESULT_SUCCESS == result) {
                // there currently is support for only one send bus, so there is a single send
                // level
                *pSendLevel = this->mSendLevel;
            }
        }
        interface_unlock_exclusive(this);
    }

    SL_LEAVE_INTERFACE
}


static const struct SLAndroidEffectSendItf_ IAndroidEffectSend_Itf = {
    IAndroidEffectSend_EnableEffectSend,
    IAndroidEffectSend_IsEnabled,
    IAndroidEffectSend_SetDirectLevel,
    IAndroidEffectSend_GetDirectLevel,
    IAndroidEffectSend_SetSendLevel,
    IAndroidEffectSend_GetSendLevel
};

void IAndroidEffectSend_init(void *self)
{
    IAndroidEffectSend *this = (IAndroidEffectSend *) self;
    this->mItf = &IAndroidEffectSend_Itf;
    this->mEnabled =  SL_BOOLEAN_FALSE;
    this->mSendLevel = SL_MILLIBEL_MIN;
}
