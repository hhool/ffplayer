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

/* AndroidConfiguration implementation */

#include "sles_allinclusive.h"


static SLresult IAndroidConfiguration_SetConfiguration(SLAndroidConfigurationItf self,
        const SLchar *configKey,
        const void *pConfigValue,
        SLuint32 valueSize)
{
    SL_ENTER_INTERFACE

    IAndroidConfiguration *this = (IAndroidConfiguration *) self;

    interface_lock_exclusive(this);

    // route configuration to the appropriate object
    if (SL_OBJECTID_AUDIORECORDER == IObjectToObjectID((this)->mThis)) {
        SL_LOGV("SetConfiguration issued for AudioRecorder key=%s valueSize=%lu",
                configKey, valueSize);
        result = android_audioRecorder_setConfig((CAudioRecorder *) this->mThis, configKey,
                pConfigValue, valueSize);
    } else if (SL_OBJECTID_AUDIOPLAYER == IObjectToObjectID((this)->mThis)) {
        SL_LOGV("SetConfiguration issued for AudioPlayer key=%s valueSize=%lu",
                configKey, valueSize);
        result = android_audioPlayer_setConfig((CAudioPlayer *) this->mThis, configKey,
                pConfigValue, valueSize);
    } else {
        result = SL_RESULT_PARAMETER_INVALID;
    }

    interface_unlock_exclusive(this);

    SL_LEAVE_INTERFACE
}


static SLresult IAndroidConfiguration_GetConfiguration(SLAndroidConfigurationItf self,
        const SLchar *configKey,
        SLuint32 *pValueSize,
        void *pConfigValue)
{
    SL_ENTER_INTERFACE

    // having value size is required, but pConfigValue being NULL is allowed to allow properties
    // to report their actual value size (if applicable)
    if (NULL == pValueSize) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        IAndroidConfiguration *this = (IAndroidConfiguration *) self;

        interface_lock_exclusive(this);

        // route configuration request to the appropriate object
        if (SL_OBJECTID_AUDIORECORDER == IObjectToObjectID((this)->mThis)) {
            result = android_audioRecorder_getConfig((CAudioRecorder *) this->mThis, configKey,
                    pValueSize, pConfigValue);
        } else if (SL_OBJECTID_AUDIOPLAYER == IObjectToObjectID((this)->mThis)) {
            result = android_audioPlayer_getConfig((CAudioPlayer *) this->mThis, configKey,
                    pValueSize, pConfigValue);
        } else {
            result = SL_RESULT_PARAMETER_INVALID;
        }

        interface_unlock_exclusive(this);
    }

    SL_LEAVE_INTERFACE
}


static const struct SLAndroidConfigurationItf_ IAndroidConfiguration_Itf = {
    IAndroidConfiguration_SetConfiguration,
    IAndroidConfiguration_GetConfiguration
};

void IAndroidConfiguration_init(void *self)
{
    IAndroidConfiguration *this = (IAndroidConfiguration *) self;
    this->mItf = &IAndroidConfiguration_Itf;
}
