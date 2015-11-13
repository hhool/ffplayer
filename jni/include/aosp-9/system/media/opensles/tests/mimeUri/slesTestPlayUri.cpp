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

/*
 * Copyright (c) 2009 The Khronos Group Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and /or associated documentation files (the "Materials "), to deal in the
 * Materials without restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies of the Materials,
 * and to permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE MATERIALS OR THE USE OR OTHER DEALINGS IN THE
 * MATERIALS.
 */

#include <stdlib.h>
#include <stdio.h>
//#include <string.h>
#include <unistd.h>
//#include <sys/time.h>

#include "SLES/OpenSLES.h"


#define MAX_NUMBER_INTERFACES 2

#define PREFETCHEVENT_ERROR_CANDIDATE \
        (SL_PREFETCHEVENT_STATUSCHANGE | SL_PREFETCHEVENT_FILLLEVELCHANGE)

//-----------------------------------------------------------------
//* Exits the application if an error is encountered */
#define CheckErr(x) ExitOnErrorFunc(x,__LINE__)

void ExitOnErrorFunc( SLresult result , int line)
{
    if (SL_RESULT_SUCCESS != result) {
        fprintf(stderr, "%lu error code encountered at line %d, exiting\n", result, line);
        exit(EXIT_FAILURE);
    }
}

bool prefetchError = false;

//-----------------------------------------------------------------
/* PrefetchStatusItf callback for an audio player */
void PrefetchEventCallback( SLPrefetchStatusItf caller,  void *pContext, SLuint32 event)
{
    SLpermille level = 0;
    (*caller)->GetFillLevel(caller, &level);
    SLuint32 status;
    //fprintf(stdout, "PrefetchEventCallback: received event %lu\n", event);
    (*caller)->GetPrefetchStatus(caller, &status);
    if ((PREFETCHEVENT_ERROR_CANDIDATE == (event & PREFETCHEVENT_ERROR_CANDIDATE))
            && (level == 0) && (status == SL_PREFETCHSTATUS_UNDERFLOW)) {
        fprintf(stdout, "PrefetchEventCallback: Error while prefetching data, exiting\n");
        prefetchError = true;
    }
    if (event & SL_PREFETCHEVENT_FILLLEVELCHANGE) {
        fprintf(stdout, "PrefetchEventCallback: Buffer fill level is = %d\n", level);
    }
    if (event & SL_PREFETCHEVENT_STATUSCHANGE) {
        fprintf(stdout, "PrefetchEventCallback: Prefetch Status is = %lu\n", status);
    }

}


//-----------------------------------------------------------------

/* Play some music from a URI  */
void TestPlayUri( SLObjectItf sl, const char* path)
{
    SLEngineItf                EngineItf;

    SLint32                    numOutputs = 0;
    SLuint32                   deviceID = 0;

    SLresult                   res;

    SLDataSource               audioSource;
    SLDataLocator_URI          uri;
    SLDataFormat_MIME          mime;

    SLDataSink                 audioSink;
    SLDataLocator_OutputMix    locator_outputmix;

    SLObjectItf                player;
    SLPlayItf                  playItf;
    SLVolumeItf                volItf;
    SLPrefetchStatusItf        prefetchItf;

    SLObjectItf                OutputMix;

    SLboolean required[MAX_NUMBER_INTERFACES];
    SLInterfaceID iidArray[MAX_NUMBER_INTERFACES];

    /* Get the SL Engine Interface which is implicit */
    res = (*sl)->GetInterface(sl, SL_IID_ENGINE, (void*)&EngineItf);
    CheckErr(res);

    /* Initialize arrays required[] and iidArray[] */
    for (int i=0 ; i < MAX_NUMBER_INTERFACES ; i++) {
        required[i] = SL_BOOLEAN_FALSE;
        iidArray[i] = SL_IID_NULL;
    }

    // Set arrays required[] and iidArray[] for VOLUME and PREFETCHSTATUS interface
    required[0] = SL_BOOLEAN_TRUE;
    iidArray[0] = SL_IID_VOLUME;
    required[1] = SL_BOOLEAN_TRUE;
    iidArray[1] = SL_IID_PREFETCHSTATUS;
    // Create Output Mix object to be used by player
    res = (*EngineItf)->CreateOutputMix(EngineItf, &OutputMix, 0,
            iidArray, required); CheckErr(res);

    // Realizing the Output Mix object in synchronous mode.
    res = (*OutputMix)->Realize(OutputMix, SL_BOOLEAN_FALSE);
    CheckErr(res);

    /* Setup the data source structure for the URI */
    uri.locatorType = SL_DATALOCATOR_URI;
    uri.URI         =  (SLchar*) path;
    mime.formatType    = SL_DATAFORMAT_MIME;
    mime.mimeType      = (SLchar*)NULL;
    mime.containerType = SL_CONTAINERTYPE_UNSPECIFIED;

    audioSource.pFormat      = (void *)&mime;
    audioSource.pLocator     = (void *)&uri;

    /* Setup the data sink structure */
    locator_outputmix.locatorType   = SL_DATALOCATOR_OUTPUTMIX;
    locator_outputmix.outputMix    = OutputMix;
    audioSink.pLocator           = (void *)&locator_outputmix;
    audioSink.pFormat            = NULL;

    /* Create the audio player */
    res = (*EngineItf)->CreateAudioPlayer(EngineItf, &player, &audioSource, &audioSink,
            MAX_NUMBER_INTERFACES, iidArray, required); CheckErr(res);

    /* Realizing the player in synchronous mode. */
    res = (*player)->Realize(player, SL_BOOLEAN_FALSE); CheckErr(res);
    fprintf(stdout, "URI example: after Realize\n");

    /* Get interfaces */
    res = (*player)->GetInterface(player, SL_IID_PLAY, (void*)&playItf);
    CheckErr(res);

    res = (*player)->GetInterface(player, SL_IID_VOLUME,  (void*)&volItf);
    CheckErr(res);

    res = (*player)->GetInterface(player, SL_IID_PREFETCHSTATUS, (void*)&prefetchItf);
    CheckErr(res);
    res = (*prefetchItf)->RegisterCallback(prefetchItf, PrefetchEventCallback, &prefetchItf);
    CheckErr(res);
    res = (*prefetchItf)->SetCallbackEventsMask(prefetchItf,
            SL_PREFETCHEVENT_FILLLEVELCHANGE | SL_PREFETCHEVENT_STATUSCHANGE);
    CheckErr(res);

    /* Configure fill level updates every 5 percent */
    (*prefetchItf)->SetFillUpdatePeriod(prefetchItf, 50);

    /* Display duration */
    SLmillisecond durationInMsec = SL_TIME_UNKNOWN;
    res = (*playItf)->GetDuration(playItf, &durationInMsec);
    CheckErr(res);
    if (durationInMsec == SL_TIME_UNKNOWN) {
        fprintf(stdout, "Content duration is unknown (before starting to prefetch)\n");
    } else {
        fprintf(stdout, "Content duration is %lu ms (before starting to prefetch)\n",
                durationInMsec);
    }

    /* Set the player volume */
    res = (*volItf)->SetVolumeLevel( volItf, -300);
    CheckErr(res);

    /* Play the URI */
    /*     first cause the player to prefetch the data */
    fprintf(stdout, "Before set to PAUSED\n");
    res = (*playItf)->SetPlayState( playItf, SL_PLAYSTATE_PAUSED );
    fprintf(stdout, "After set to PAUSED\n");
    CheckErr(res);

    /*     wait until there's data to play */
    //SLpermille fillLevel = 0;
    SLuint32 prefetchStatus = SL_PREFETCHSTATUS_UNDERFLOW;
    SLuint32 timeOutIndex = 100; // 10s
    while ((prefetchStatus != SL_PREFETCHSTATUS_SUFFICIENTDATA) && (timeOutIndex > 0) &&
            !prefetchError) {
        usleep(100 * 1000);
        (*prefetchItf)->GetPrefetchStatus(prefetchItf, &prefetchStatus);
        timeOutIndex--;
    }

    if (timeOutIndex == 0 || prefetchError) {
        fprintf(stderr, "We\'re done waiting, failed to prefetch data in time, exiting\n");
        goto destroyRes;
    }

    /* Display duration again, */
    res = (*playItf)->GetDuration(playItf, &durationInMsec);
    CheckErr(res);
    if (durationInMsec == SL_TIME_UNKNOWN) {
        fprintf(stdout, "Content duration is unknown (after prefetch completed)\n");
    } else {
        fprintf(stdout, "Content duration is %lu ms (after prefetch completed)\n", durationInMsec);
    }

    fprintf(stdout, "URI example: starting to play\n");
    res = (*playItf)->SetPlayState( playItf, SL_PLAYSTATE_PLAYING );
    CheckErr(res);

    /* Wait as long as the duration of the content before stopping */
    usleep(durationInMsec * 1000);

    /* Make sure player is stopped */
    fprintf(stdout, "URI example: stopping playback\n");
    res = (*playItf)->SetPlayState(playItf, SL_PLAYSTATE_STOPPED);
    CheckErr(res);

destroyRes:

    /* Destroy the player */
    (*player)->Destroy(player);

    /* Destroy Output Mix object */
    (*OutputMix)->Destroy(OutputMix);
}

//-----------------------------------------------------------------
int main(int argc, char* const argv[])
{
    SLresult    res;
    SLObjectItf sl;

    fprintf(stdout, "OpenSL ES test %s: exercises SLPlayItf, SLVolumeItf ", argv[0]);
    fprintf(stdout, "and AudioPlayer with SLDataLocator_URI source / OutputMix sink\n");
    fprintf(stdout, "Plays a sound and stops after its reported duration\n\n");

    if (argc == 1) {
        fprintf(stdout, "Usage: %s path \n\t%s url\n", argv[0], argv[0]);
        fprintf(stdout, "Example: \"%s /sdcard/my.mp3\"  or \"%s file:///sdcard/my.mp3\"\n",
                argv[0], argv[0]);
        exit(EXIT_FAILURE);
    }

    SLEngineOption EngineOption[] = {
            {(SLuint32) SL_ENGINEOPTION_THREADSAFE,
            (SLuint32) SL_BOOLEAN_TRUE}};

    res = slCreateEngine( &sl, 1, EngineOption, 0, NULL, NULL);
    CheckErr(res);
    /* Realizing the SL Engine in synchronous mode. */
    res = (*sl)->Realize(sl, SL_BOOLEAN_FALSE);
    CheckErr(res);

    TestPlayUri(sl, argv[1]);

    /* Shutdown OpenSL ES */
    (*sl)->Destroy(sl);

    return EXIT_SUCCESS;
}
