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

// Demonstrate environmental reverb and preset reverb on an output mix and audio player

#include "SLES/OpenSLES.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define bool int
#define false 0
#define true 1

// Table of I3DL2 named environmental reverb settings

typedef struct {
    const char *mName;
    SLEnvironmentalReverbSettings mSettings;
} Pair;

#define _(name) {#name, SL_I3DL2_ENVIRONMENT_PRESET_##name},

Pair pairs[] = {
    _(DEFAULT)
    _(GENERIC)
    _(PADDEDCELL)
    _(ROOM)
    _(BATHROOM)
    _(LIVINGROOM)
    _(STONEROOM)
    _(AUDITORIUM)
    _(CONCERTHALL)
    _(CAVE)
    _(ARENA)
    _(HANGAR)
    _(CARPETEDHALLWAY)
    _(HALLWAY)
    _(STONECORRIDOR)
    _(ALLEY)
    _(FOREST)
    _(CITY)
    _(MOUNTAINS)
    _(QUARRY)
    _(PLAIN)
    _(PARKINGLOT)
    _(SEWERPIPE)
    _(UNDERWATER)
    _(SMALLROOM)
    _(MEDIUMROOM)
    _(LARGEROOM)
    _(MEDIUMHALL)
    _(LARGEHALL)
    _(PLATE)
};

// Reverb parameters for output mix
SLuint16 mixPresetNumber = ~0;
char *mixEnvName = NULL;
SLEnvironmentalReverbSettings mixEnvSettings;

// Reverb parameters for audio player
SLuint16 playerPresetNumber = ~0;
char *playerEnvName = NULL;
SLEnvironmentalReverbSettings playerEnvSettings;

// Compare two environmental reverb settings structures.
// Returns true if the settings are identical, or false if they are different.

bool slesutCompareEnvronmentalReverbSettings(
        const SLEnvironmentalReverbSettings *settings1,
        const SLEnvironmentalReverbSettings *settings2)
{
    return
        (settings1->roomLevel == settings2->roomLevel) &&
        (settings1->roomHFLevel == settings2->roomHFLevel) &&
        (settings1->decayTime == settings2->decayTime) &&
        (settings1->decayHFRatio == settings2->decayHFRatio) &&
        (settings1->reflectionsLevel == settings2->reflectionsLevel) &&
        (settings1->reflectionsDelay == settings2->reflectionsDelay) &&
        (settings1->reverbLevel == settings2->reverbLevel) &&
        (settings1->reverbDelay == settings2->reverbDelay) &&
        (settings1->diffusion == settings2->diffusion) &&
        (settings1->density == settings2->density);
}

// Print an environmental reverb settings structure.

void slesutPrintEnvironmentalReverbSettings(const SLEnvironmentalReverbSettings *settings)
{
    printf("roomLevel: %u\n", settings->roomLevel);
    printf("roomHFLevel: %u\n", settings->roomHFLevel);
    printf("decayTime: %lu\n", settings->decayTime);
    printf("decayHFRatio: %u\n", settings->decayHFRatio);
    printf("reflectionsLevel: %u\n", settings->reflectionsLevel);
    printf("reflectionsDelay: %lu\n", settings->reflectionsDelay);
    printf("reverbLevel: %u\n", settings->reverbLevel);
    printf("reverbDelay: %lu\n", settings->reverbDelay);
    printf("diffusion: %u\n", settings->diffusion);
    printf("density: %u\n", settings->density);
}

// Lookup environmental reverb settings by name

const SLEnvironmentalReverbSettings *lookupEnvName(const char *name)
{
    unsigned j;
    for (j = 0; j < sizeof(pairs) / sizeof(pairs[0]); ++j) {
        if (!strcasecmp(name, pairs[j].mName)) {
            return &pairs[j].mSettings;
        }
    }
    return NULL;
}

// Print all available environmental reverb names

void printEnvNames(void)
{
    unsigned j;
    bool needSpace = false;
    bool needNewline = false;
    unsigned lineLen = 0;
    for (j = 0; j < sizeof(pairs) / sizeof(pairs[0]); ++j) {
        const char *name = pairs[j].mName;
        unsigned nameLen = strlen(name);
        if (lineLen + (needSpace ? 1 : 0) + nameLen > 72) {
            putchar('\n');
            needSpace = false;
            needNewline = false;
            lineLen = 0;
        }
        if (needSpace) {
            putchar(' ');
            ++lineLen;
        }
        fputs(name, stdout);
        lineLen += nameLen;
        needSpace = true;
        needNewline = true;
    }
    if (needNewline) {
        putchar('\n');
    }
}

// Main program

int main(int argc, char **argv)
{
    SLresult result;

    // process command line parameters
    char *prog = argv[0];
    int i;
    for (i = 1; i < argc; ++i) {
        char *arg = argv[i];
        if (arg[0] != '-')
            break;
        if (!strncmp(arg, "--mix-preset=", 13)) {
            mixPresetNumber = atoi(&arg[13]);
        } else if (!strncmp(arg, "--mix-name=", 11)) {
            mixEnvName = &arg[11];
        } else if (!strncmp(arg, "--player-preset=", 16)) {
            playerPresetNumber = atoi(&arg[16]);
        } else if (!strncmp(arg, "--player-name=", 14)) {
            playerEnvName = &arg[14];
        } else {
            fprintf(stderr, "%s: unknown option %s ignored\n", prog, arg);
        }
    }
    if (argc - i != 1) {
        fprintf(stderr, "usage: %s --mix-preset=# --mix-name=I3DL2 --player-preset=# "
                "--player-name=I3DL2 filename\n", prog);
        return EXIT_FAILURE;
    }
    char *pathname = argv[i];

    const SLEnvironmentalReverbSettings *envSettings;
    if (NULL != mixEnvName) {
        envSettings = lookupEnvName(mixEnvName);
        if (NULL == envSettings) {
            fprintf(stderr, "%s: mix environmental reverb name %s not found, "
                    "available names are:\n", prog, mixEnvName);
            printEnvNames();
            return EXIT_FAILURE;
        }
        mixEnvSettings = *envSettings;
    }
    if (NULL != playerEnvName) {
        envSettings = lookupEnvName(playerEnvName);
        if (NULL == envSettings) {
            fprintf(stderr, "%s: player environmental reverb name %s not found, "
                    "available names are:\n", prog, playerEnvName);
            printEnvNames();
            return EXIT_FAILURE;
        }
        playerEnvSettings = *envSettings;
    }

    // create engine
    SLObjectItf engineObject;
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    assert(SL_RESULT_SUCCESS == result);
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    SLEngineItf engineEngine;
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    assert(SL_RESULT_SUCCESS == result);

    // create output mix
    SLInterfaceID mix_ids[2];
    SLboolean mix_req[2];
    SLuint32 count = 0;
    if (mixPresetNumber != ((SLuint16) ~0)) {
        mix_req[count] = SL_BOOLEAN_TRUE;
        mix_ids[count++] = SL_IID_PRESETREVERB;
    }
    if (mixEnvName != NULL) {
        mix_req[count] = SL_BOOLEAN_TRUE;
        mix_ids[count++] = SL_IID_ENVIRONMENTALREVERB;
    }
    SLObjectItf mixObject;
    result = (*engineEngine)->CreateOutputMix(engineEngine, &mixObject, count, mix_ids, mix_req);
    assert(SL_RESULT_SUCCESS == result);
    result = (*mixObject)->Realize(mixObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);

    // configure preset reverb on output mix
    SLPresetReverbItf mixPresetReverb;
    if (mixPresetNumber != ((SLuint16) ~0)) {
        result = (*mixObject)->GetInterface(mixObject, SL_IID_PRESETREVERB, &mixPresetReverb);
        assert(SL_RESULT_SUCCESS == result);
        SLuint16 getPresetReverb = 12345;
        result = (*mixPresetReverb)->GetPreset(mixPresetReverb, &getPresetReverb);
        assert(SL_RESULT_SUCCESS == result);
        printf("Output mix default preset reverb %u\n", getPresetReverb);
        result = (*mixPresetReverb)->SetPreset(mixPresetReverb, mixPresetNumber);
        if (SL_RESULT_SUCCESS == result) {
            result = (*mixPresetReverb)->GetPreset(mixPresetReverb, &getPresetReverb);
            assert(SL_RESULT_SUCCESS == result);
            assert(getPresetReverb == mixPresetNumber);
            printf("Output mix preset reverb successfully changed to %u\n", mixPresetNumber);
        } else
            printf("Unable to set output mix preset reverb to %u, result=%lu\n", mixPresetNumber,
                    result);
    }

    // configure environmental reverb on output mix
    SLEnvironmentalReverbItf mixEnvironmentalReverb;
    if (mixEnvName != NULL) {
        result = (*mixObject)->GetInterface(mixObject, SL_IID_ENVIRONMENTALREVERB,
                &mixEnvironmentalReverb);
        assert(SL_RESULT_SUCCESS == result);
        SLEnvironmentalReverbSettings getSettings;
        result = (*mixEnvironmentalReverb)->GetEnvironmentalReverbProperties(mixEnvironmentalReverb,
                &getSettings);
        assert(SL_RESULT_SUCCESS == result);
        printf("Output mix default environmental reverb settings\n");
        printf("------------------------------------------------\n");
        slesutPrintEnvironmentalReverbSettings(&getSettings);
        printf("\n");
        result = (*mixEnvironmentalReverb)->SetEnvironmentalReverbProperties(mixEnvironmentalReverb,
                &mixEnvSettings);
        assert(SL_RESULT_SUCCESS == result);
        printf("Output mix new environmental reverb settings\n");
        printf("--------------------------------------------\n");
        slesutPrintEnvironmentalReverbSettings(&mixEnvSettings);
        printf("\n");
        result = (*mixEnvironmentalReverb)->GetEnvironmentalReverbProperties(mixEnvironmentalReverb,
                &getSettings);
        assert(SL_RESULT_SUCCESS == result);
        printf("Output mix read environmental reverb settings\n");
        printf("--------------------------------------------\n");
        slesutPrintEnvironmentalReverbSettings(&getSettings);
        printf("\n");
        if (!slesutCompareEnvronmentalReverbSettings(&getSettings, &mixEnvSettings)) {
            printf("Warning: new and read are different; check details above\n");
        } else {
            printf("New and read match, life is good\n");
        }
    }

    // create audio player
    SLDataLocator_URI locURI = {SL_DATALOCATOR_URI, (SLchar *) pathname};
    SLDataFormat_MIME dfMIME = {SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED};
    SLDataSource audioSrc = {&locURI, &dfMIME};
    SLDataLocator_OutputMix locOutputMix = {SL_DATALOCATOR_OUTPUTMIX, mixObject};
    SLDataSink audioSnk = {&locOutputMix, NULL};
    SLInterfaceID player_ids[4];
    SLboolean player_req[4];
    count = 0;
    if (playerPresetNumber != ((SLuint16) ~0)) {
        player_req[count] = SL_BOOLEAN_TRUE;
        player_ids[count++] = SL_IID_PRESETREVERB;
    }
    if (playerEnvName != NULL) {
        player_req[count] = SL_BOOLEAN_TRUE;
        player_ids[count++] = SL_IID_ENVIRONMENTALREVERB;
    }
    if (mixPresetNumber != ((SLuint16) ~0) || mixEnvName != NULL) {
        player_req[count] = SL_BOOLEAN_TRUE;
        player_ids[count++] = SL_IID_EFFECTSEND;
    }
    player_req[count] = SL_BOOLEAN_TRUE;
    player_ids[count++] = SL_IID_PREFETCHSTATUS;
    SLObjectItf playerObject;
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &playerObject, &audioSrc,
        &audioSnk, count, player_ids, player_req);
    assert(SL_RESULT_SUCCESS == result);

    // realize audio player
    result = (*playerObject)->Realize(playerObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);

    // if reverb is on output mix (aux effect), then enable it for this player
    if (mixPresetNumber != ((SLuint16) ~0) || mixEnvName != NULL) {
        SLEffectSendItf playerEffectSend;
        result = (*playerObject)->GetInterface(playerObject, SL_IID_EFFECTSEND, &playerEffectSend);
        assert(SL_RESULT_SUCCESS == result);
        if (mixPresetNumber != ((SLuint16) ~0)) {
            result = (*playerEffectSend)->EnableEffectSend(playerEffectSend, mixPresetReverb,
                    SL_BOOLEAN_TRUE, (SLmillibel) 0);
            assert(SL_RESULT_SUCCESS == result);
        }
        if (mixEnvName != NULL) {
            result = (*playerEffectSend)->EnableEffectSend(playerEffectSend, mixEnvironmentalReverb,
                    SL_BOOLEAN_TRUE, (SLmillibel) 0);
            assert(SL_RESULT_SUCCESS == result);
        }
    }

    // configure preset reverb on player
    SLPresetReverbItf playerPresetReverb;
    if (playerPresetNumber != ((SLuint16) ~0)) {
        result = (*playerObject)->GetInterface(playerObject, SL_IID_PRESETREVERB,
                &playerPresetReverb);
        assert(SL_RESULT_SUCCESS == result);
        SLuint16 getPresetReverb = 12345;
        result = (*playerPresetReverb)->GetPreset(playerPresetReverb, &getPresetReverb);
        assert(SL_RESULT_SUCCESS == result);
        printf("Player default preset reverb %u\n", getPresetReverb);
        result = (*playerPresetReverb)->SetPreset(playerPresetReverb, playerPresetNumber);
        if (SL_RESULT_SUCCESS == result) {
            result = (*playerPresetReverb)->GetPreset(playerPresetReverb, &getPresetReverb);
            assert(SL_RESULT_SUCCESS == result);
            assert(getPresetReverb == playerPresetNumber);
            printf("Player preset reverb successfully changed to %u\n", playerPresetNumber);
        } else
            printf("Unable to set player preset reverb to %u, result=%lu\n", playerPresetNumber,
                    result);
    }

    // configure environmental reverb on player
    SLEnvironmentalReverbItf playerEnvironmentalReverb;
    if (playerEnvName != NULL) {
        result = (*playerObject)->GetInterface(playerObject, SL_IID_ENVIRONMENTALREVERB,
                &playerEnvironmentalReverb);
        assert(SL_RESULT_SUCCESS == result);
        SLEnvironmentalReverbSettings getSettings;
        memset(&getSettings, 0, sizeof(getSettings));
        result = (*playerEnvironmentalReverb)->GetEnvironmentalReverbProperties(
                playerEnvironmentalReverb, &getSettings);
        assert(SL_RESULT_SUCCESS == result);
        printf("Player default environmental reverb settings\n");
        printf("--------------------------------------------\n");
        slesutPrintEnvironmentalReverbSettings(&getSettings);
        printf("\n");
        result = (*playerEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                playerEnvironmentalReverb, &playerEnvSettings);
        assert(SL_RESULT_SUCCESS == result);
        printf("Player new environmental reverb settings\n");
        printf("----------------------------------------\n");
        slesutPrintEnvironmentalReverbSettings(&playerEnvSettings);
        printf("\n");
        result = (*playerEnvironmentalReverb)->GetEnvironmentalReverbProperties(
                playerEnvironmentalReverb, &getSettings);
        assert(SL_RESULT_SUCCESS == result);
        printf("Player read environmental reverb settings\n");
        printf("-----------------------------------------\n");
        slesutPrintEnvironmentalReverbSettings(&getSettings);
        printf("\n");
        if (!slesutCompareEnvronmentalReverbSettings(&getSettings, &playerEnvSettings)) {
            printf("Warning: new and read are different; check details above\n");
        } else {
            printf("New and read match, life is good\n");
        }
    }

    // get the play interface
    SLPlayItf playerPlay;
    result = (*playerObject)->GetInterface(playerObject, SL_IID_PLAY, &playerPlay);
    assert(SL_RESULT_SUCCESS == result);

    // set play state to paused to enable pre-fetch so we can get a more reliable duration
    result = (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_PAUSED);
    assert(SL_RESULT_SUCCESS == result);

    // get the prefetch status interface
    SLPrefetchStatusItf playerPrefetchStatus;
    result = (*playerObject)->GetInterface(playerObject, SL_IID_PREFETCHSTATUS,
            &playerPrefetchStatus);
    assert(SL_RESULT_SUCCESS == result);

    // poll prefetch status to detect when it completes
    SLuint32 prefetchStatus = SL_PREFETCHSTATUS_UNDERFLOW;
    SLuint32 timeOutIndex = 100; // 10s
    while ((prefetchStatus != SL_PREFETCHSTATUS_SUFFICIENTDATA) && (timeOutIndex > 0)) {
        usleep(100 * 1000);
        (*playerPrefetchStatus)->GetPrefetchStatus(playerPrefetchStatus, &prefetchStatus);
        timeOutIndex--;
    }
    if (timeOutIndex == 0) {
        fprintf(stderr, "\nWe\'re done waiting, failed to prefetch data in time, exiting\n");
        goto destroyRes;
    }

    // get the duration
    SLmillisecond duration;
    result = (*playerPlay)->GetDuration(playerPlay, &duration);
    assert(SL_RESULT_SUCCESS == result);
    if (SL_TIME_UNKNOWN == duration) {
        printf("duration: unknown\n");
    } else {
        printf("duration: %.1f seconds\n", duration / 1000.0);
    }

    // start audio playing
    result = (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_PLAYING);
    assert(SL_RESULT_SUCCESS == result);

    // wait for audio to finish playing
    SLuint32 state;
    for (;;) {
        result = (*playerPlay)->GetPlayState(playerPlay, &state);
        assert(SL_RESULT_SUCCESS == result);
        if (SL_PLAYSTATE_PLAYING != state)
            break;
        usleep(1000000);
     }
    assert(SL_PLAYSTATE_PAUSED == state);

destroyRes:
    // cleanup objects
    (*playerObject)->Destroy(playerObject);
    (*mixObject)->Destroy(mixObject);
    (*engineObject)->Destroy(engineObject);

    return EXIT_SUCCESS;
}
