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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "SLES/OpenSLES.h"


#define MAX_NUMBER_INTERFACES 3

#define TEST_MUTE 0
#define TEST_SOLO 1

typedef struct {
    int testMode;
    SLPlayItf playItf;
    SLMuteSoloItf muteSoloItf;
} Context;

//-----------------------------------------------------------------
/* Exits the application if an error is encountered */
#define ExitOnError(x) ExitOnErrorFunc(x,__LINE__)

void ExitOnErrorFunc( SLresult result , int line)
{
    if (SL_RESULT_SUCCESS != result) {
        fprintf(stdout, "%lu error code encountered at line %d, exiting\n", result, line);
        exit(EXIT_FAILURE);
    }
}

//-----------------------------------------------------------------
/* PlayItf callback for an audio player, will be called for every SL_PLAYEVENT_HEADATNEWPOS event */
void PlayEventCallback( SLPlayItf caller,  void *pContext, SLuint32 event)
{
    Context *context = (Context *) pContext;
    SLPlayItf playItf = context->playItf;
    SLMuteSoloItf muteSolo = context->muteSoloItf;
    SLuint8 numChannels = 0;
    SLresult res = (*muteSolo)->GetNumChannels(muteSolo, &numChannels); ExitOnError(res);
    //fprintf(stdout, "Content has %d channel(s)\n", numChannels);
    SLmillisecond position;
    res = (*playItf)->GetPosition(playItf, &position); ExitOnError(res);
    printf("position=%u\n", (unsigned) position);

    switch (context->testMode) {
        case TEST_MUTE: {
            //---------------------------------------------------
            if (numChannels > 1) { // SLMuteSoloItf only works if more than one channel
                SLboolean leftMuted = SL_BOOLEAN_TRUE;
                res = (*muteSolo)->GetChannelMute(muteSolo, 0, &leftMuted); ExitOnError(res);
                // swap channel mute
                res = (*muteSolo)->SetChannelMute(muteSolo, 0,
                       leftMuted == SL_BOOLEAN_TRUE ? SL_BOOLEAN_FALSE : SL_BOOLEAN_TRUE);
                ExitOnError(res);
                res = (*muteSolo)->SetChannelMute(muteSolo, 1,
                       leftMuted == SL_BOOLEAN_TRUE ? SL_BOOLEAN_TRUE : SL_BOOLEAN_FALSE);
                ExitOnError(res);
                if (leftMuted == SL_BOOLEAN_TRUE) { // we've swapped the channel mute above
                    fprintf(stdout, "channel 0: playing, channel 1: muted\n");
                } else {
                    fprintf(stdout, "channel 0: muted, channel 1: playing\n");
                }
            }
            } break;

        case TEST_SOLO: {
            //---------------------------------------------------
            if (numChannels > 1) { // SLMuteSoloItf only works if more than one channel
                SLboolean leftSoloed = SL_BOOLEAN_TRUE;
                res = (*muteSolo)->GetChannelSolo(muteSolo, 0, &leftSoloed); ExitOnError(res);
                // swap channel solo
                res = (*muteSolo)->SetChannelSolo(muteSolo, 0,
                        leftSoloed == SL_BOOLEAN_TRUE ? SL_BOOLEAN_FALSE : SL_BOOLEAN_TRUE);
                ExitOnError(res);
                res = (*muteSolo)->SetChannelSolo(muteSolo, 1,
                        leftSoloed == SL_BOOLEAN_TRUE ? SL_BOOLEAN_TRUE : SL_BOOLEAN_FALSE);
                ExitOnError(res);
                if (leftSoloed == SL_BOOLEAN_TRUE) { // we've swapped the channel solo above
                    fprintf(stdout, "channel 0: normal, channel 1: soloed\n");
                } else {
                    fprintf(stdout, "channel 0: soloed, channel 1: normal\n");
                }
            }
            } break;

        default:
            break;
    }
}

//-----------------------------------------------------------------

/* Play an audio URIs, mute and solo channels  */
void TestPlayUri( SLObjectItf sl, const char* path)
{
    SLresult  result;
    SLEngineItf EngineItf;

    /* Objects this application uses: one player and an ouput mix */
    SLObjectItf  player, outputMix;

    /* Source of audio data to play */
    SLDataSource      audioSource;
    SLDataLocator_URI uri;
    SLDataFormat_MIME mime;

    /* Data sinks for the audio player */
    SLDataSink               audioSink;
    SLDataLocator_OutputMix  locator_outputmix;

    /* Play, Volume and PrefetchStatus interfaces for the audio player */
    SLPlayItf           playItf;
    SLMuteSoloItf       muteSoloItf;
    SLPrefetchStatusItf prefetchItf;

    SLboolean required[MAX_NUMBER_INTERFACES];
    SLInterfaceID iidArray[MAX_NUMBER_INTERFACES];

    /* Get the SL Engine Interface which is implicit */
    result = (*sl)->GetInterface(sl, SL_IID_ENGINE, (void*)&EngineItf);
    ExitOnError(result);

    /* Initialize arrays required[] and iidArray[] */
    for (int i=0 ; i < MAX_NUMBER_INTERFACES ; i++) {
        required[i] = SL_BOOLEAN_FALSE;
        iidArray[i] = SL_IID_NULL;
    }

    /* ------------------------------------------------------ */
    /* Configuration of the output mix  */

    /* Create Output Mix object to be used by the player */
     result = (*EngineItf)->CreateOutputMix(EngineItf, &outputMix, 0, iidArray, required);
     ExitOnError(result);

    /* Realize the Output Mix object in synchronous mode */
    result = (*outputMix)->Realize(outputMix, SL_BOOLEAN_FALSE);
    ExitOnError(result);

    /* Setup the data sink structure */
    locator_outputmix.locatorType = SL_DATALOCATOR_OUTPUTMIX;
    locator_outputmix.outputMix   = outputMix;
    audioSink.pLocator            = (void*)&locator_outputmix;
    audioSink.pFormat             = NULL;

    /* ------------------------------------------------------ */
    /* Configuration of the player  */

    /* Set arrays required[] and iidArray[] for SLMuteSoloItf and SLPrefetchStatusItf interfaces */
    /*  (SLPlayItf is implicit) */
    required[0] = SL_BOOLEAN_TRUE;
    iidArray[0] = SL_IID_MUTESOLO;
    required[1] = SL_BOOLEAN_TRUE;
    iidArray[1] = SL_IID_PREFETCHSTATUS;

    /* Setup the data source structure for the URI */
    uri.locatorType = SL_DATALOCATOR_URI;
    uri.URI         =  (SLchar*) path;
    mime.formatType = SL_DATAFORMAT_MIME;
    /*     this is how ignored mime information is specified, according to OpenSL ES spec
     *     in 9.1.6 SLDataFormat_MIME and 8.23 SLMetadataTraversalItf GetChildInfo */
    mime.mimeType      = (SLchar*)NULL;
    mime.containerType = SL_CONTAINERTYPE_UNSPECIFIED;

    audioSource.pFormat  = (void*)&mime;
    audioSource.pLocator = (void*)&uri;

    /* Create the audio player */
    result = (*EngineItf)->CreateAudioPlayer(EngineItf, &player, &audioSource, &audioSink, 2,
            iidArray, required);
    ExitOnError(result);

    /* Realize the player in synchronous mode. */
    result = (*player)->Realize(player, SL_BOOLEAN_FALSE); ExitOnError(result);
    fprintf(stdout, "URI example: after Realize\n");

    /* Get the SLPlayItf, SLPrefetchStatusItf and SLMuteSoloItf interfaces for the player */
    result = (*player)->GetInterface(player, SL_IID_PLAY, (void*)&playItf);
    ExitOnError(result);

    result = (*player)->GetInterface(player, SL_IID_PREFETCHSTATUS, (void*)&prefetchItf);
    ExitOnError(result);

    result = (*player)->GetInterface(player, SL_IID_MUTESOLO, (void*)&muteSoloItf);
    ExitOnError(result);

    // Attempt to get the channel count before it is necessarily known.
    // This may fail depending on the platform.
    SLuint8 numChannels = 123;
    result = (*muteSoloItf)->GetNumChannels(muteSoloItf, &numChannels);
    printf("GetNumChannels after Realize but before pre-fetch: result=%lu, numChannels=%u\n",
        result, numChannels);

    /* Initialize a context for use by the callback */
    Context             context;
    context.playItf = playItf;
    context.muteSoloItf = muteSoloItf;
    context.testMode = TEST_MUTE;

    /*  Setup to receive playback events on position updates */
    result = (*playItf)->RegisterCallback(playItf, PlayEventCallback, (void *) &context);
    ExitOnError(result);
    result = (*playItf)->SetCallbackEventsMask(playItf, SL_PLAYEVENT_HEADATNEWPOS);
    ExitOnError(result);
    result = (*playItf)->SetPositionUpdatePeriod(playItf, 1000);
    ExitOnError(result);

    fprintf(stdout, "Player configured\n");

    /* ------------------------------------------------------ */
    /* Playback and test */

    /* Start the data prefetching by setting the player to the paused state */
    result = (*playItf)->SetPlayState( playItf, SL_PLAYSTATE_PAUSED );
    ExitOnError(result);

    /* Wait until there's data to play */
    SLuint32 prefetchStatus = SL_PREFETCHSTATUS_UNDERFLOW;
    while (prefetchStatus != SL_PREFETCHSTATUS_SUFFICIENTDATA) {
        usleep(100 * 1000);
        (*prefetchItf)->GetPrefetchStatus(prefetchItf, &prefetchStatus);
    }

    /* Query the number of channels */
    numChannels = 123;
    result = (*muteSoloItf)->GetNumChannels(muteSoloItf, &numChannels);
    ExitOnError(result);
    fprintf(stdout, "Content has %d channel(s)\n", numChannels);

    if (numChannels == 1) {
        fprintf(stdout, "SLMuteSolotItf only works one content with more than one channel. Bye\n");
        goto destroyKillKill;
    } else {
        /* Mute left channel */
        result = (*muteSoloItf)->SetChannelMute(muteSoloItf, 0, SL_BOOLEAN_TRUE);
        ExitOnError(result);
        result = (*muteSoloItf)->SetChannelMute(muteSoloItf, 1, SL_BOOLEAN_FALSE);
        ExitOnError(result);
    }

    /* Run the test for 10s */
    /* see PlayEventCallback() for more of the test of the SLMuteSoloItf interface */
    fprintf(stdout, "\nTesting mute functionality:\n");
    context.testMode = TEST_MUTE;
    result = (*playItf)->SetPlayState( playItf, SL_PLAYSTATE_PLAYING ); ExitOnError(result);
    usleep( 5 * 1000 * 1000);
    result = (*muteSoloItf)->SetChannelMute(muteSoloItf, 0, SL_BOOLEAN_FALSE); ExitOnError(result);
    result = (*muteSoloItf)->SetChannelMute(muteSoloItf, 1, SL_BOOLEAN_FALSE); ExitOnError(result);
    fprintf(stdout, "\nTesting solo functionality:\n");
    context.testMode = TEST_SOLO;
    usleep( 5 * 1000 * 1000);

    /* Make sure player is stopped */
    fprintf(stdout, "Stopping playback\n");
    result = (*playItf)->SetPlayState(playItf, SL_PLAYSTATE_STOPPED);
    ExitOnError(result);

destroyKillKill:

    /* Destroy the players */
    (*player)->Destroy(player);

    /* Destroy Output Mix object */
    (*outputMix)->Destroy(outputMix);
}

//-----------------------------------------------------------------
int main(int argc, char* const argv[])
{
    SLresult    result;
    SLObjectItf sl;

    fprintf(stdout, "OpenSL ES test %s: exercises SLPlayItf, SLVolumeItf, SLMuteSoloItf\n",
            argv[0]);
    fprintf(stdout, "and AudioPlayer with SLDataLocator_URI source / OutputMix sink\n");
    fprintf(stdout, "Plays a sound and alternates the muting of the channels (for 5s).\n");
    fprintf(stdout, " and then alternates the solo\'ing of the channels (for 5s).\n");
    fprintf(stdout, "Stops after 10s\n");

    if (argc == 1) {
        fprintf(stdout, "Usage: \t%s url\n", argv[0]);
        fprintf(stdout, "Example: \"%s /sdcard/my.mp3\"\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    SLEngineOption EngineOption[] = {
            {(SLuint32) SL_ENGINEOPTION_THREADSAFE, (SLuint32) SL_BOOLEAN_TRUE}
    };

    result = slCreateEngine( &sl, 1, EngineOption, 0, NULL, NULL);
    ExitOnError(result);

    /* Realizing the SL Engine in synchronous mode. */
    result = (*sl)->Realize(sl, SL_BOOLEAN_FALSE);
    ExitOnError(result);

    if (argc > 1) {
        TestPlayUri(sl, argv[1]);
    }

    /* Shutdown OpenSL ES */
    (*sl)->Destroy(sl);

    return EXIT_SUCCESS;
}
