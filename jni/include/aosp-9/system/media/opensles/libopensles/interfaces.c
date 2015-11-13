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

// Map minimal perfect hash of an interface ID to its name

#include "MPH.h"

const char * const interface_names[MPH_MAX] = {
    "3DCOMMIT",
    "3DDOPPLER",
    "3DGROUPING",
    "3DLOCATION",
    "3DMACROSCOPIC",
    "3DSOURCE",
    "AUDIODECODERCAPABILITIES",
    "AUDIOENCODER",
    "AUDIOENCODERCAPABILITIES",
    "AUDIOIODEVICECAPABILITIES",
    "BASSBOOST",
    "BUFFERQUEUE",
    "DEVICEVOLUME",
    "DYNAMICINTERFACEMANAGEMENT",
    "DYNAMICSOURCE",
    "EFFECTSEND",
    "ENGINE",
    "ENGINECAPABILITIES",
    "ENVIRONMENTALREVERB",
    "EQUALIZER",
    "LED",
    "METADATAEXTRACTION",
    "METADATATRAVERSAL",
    "MIDIMESSAGE",
    "MIDIMUTESOLO",
    "MIDITEMPO",
    "MIDITIME",
    "MUTESOLO",
    "NULL",
    "OBJECT",
    "OUTPUTMIX",
    "PITCH",
    "PLAY",
    "PLAYBACKRATE",
    "PREFETCHSTATUS",
    "PRESETREVERB",
    "RATEPITCH",
    "RECORD",
    "SEEK",
    "THREADSYNC",
    "VIBRA",
    "VIRTUALIZER",
    "VISUALIZATION",
    "VOLUME",
    // The lack of ifdef is intentional
    "OUTPUTMIXEXT",
    "ANDROIDEFFECT",
    "ANDROIDEFFECTCAPABILITIES",
    "ANDROIDEFFECTSEND",
    "ANDROIDCONFIGURATION",
    "ANDROIDSIMPLEBUFFERQUEUE"
};
