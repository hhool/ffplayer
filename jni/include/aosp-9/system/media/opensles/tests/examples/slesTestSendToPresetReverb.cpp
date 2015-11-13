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
#include <fcntl.h>

#include "SLES/OpenSLES.h"
#ifdef ANDROID
#include "SLES/OpenSLES_Android.h"
#endif


#define MAX_NUMBER_INTERFACES 3

#define TIME_S_BETWEEN_SETTING_CHANGE 5

//-----------------------------------------------------------------
/* Exits the application if an error is encountered */
#define ExitOnError(x) ExitOnErrorFunc(x,__LINE__)

void ExitOnErrorFunc( SLresult result , int line)
{
    if (SL_RESULT_SUCCESS != result) {
        fprintf(stderr, "%lu error code encountered at line %d, exiting\n", result, line);
        exit(EXIT_FAILURE);
    }
}


//-----------------------------------------------------------------

/* Play an audio path and feed a global reverb  */
void TestSendToPresetReverb( SLObjectItf sl, const char* path, int preset, SLmillibel directLevel,
        SLmillibel sendLevel)
{
    SLresult  result;
    SLEngineItf EngineItf;

    /* Objects this application uses: one player and an ouput mix */
    SLObjectItf  player, outputMix;

    /* Source of audio data to play */
    SLDataSource            audioSource;
#ifdef ANDROID
    SLDataLocator_AndroidFD locatorFd;
#else
    SLDataLocator_URI       locatorUri;
#endif
    SLDataFormat_MIME       mime;

    /* Data sinks for the audio player */
    SLDataSink               audioSink;
    SLDataLocator_OutputMix  locator_outputmix;

    /* Interfaces for the audio player */
    SLPlayItf              playItf;
    SLEffectSendItf        effectSendItf;

    /* Interface for the output mix */
    SLPresetReverbItf      reverbItf;

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

    /* Set arrays required[] and iidArray[] for required interfaces */
    required[0] = SL_BOOLEAN_TRUE;
    iidArray[0] = SL_IID_PRESETREVERB;

    /* Create Output Mix object to be used by the player */
     result = (*EngineItf)->CreateOutputMix(EngineItf, &outputMix, 1, iidArray, required);
     ExitOnError(result);

    /* Realize the Output Mix object in synchronous mode */
    result = (*outputMix)->Realize(outputMix, SL_BOOLEAN_FALSE);
    ExitOnError(result);

    /* Get the SLPresetReverbItf for the output mix */
    result = (*outputMix)->GetInterface(outputMix, SL_IID_PRESETREVERB, (void*)&reverbItf);
    ExitOnError(result);

    /* Setup the data sink structure */
    locator_outputmix.locatorType = SL_DATALOCATOR_OUTPUTMIX;
    locator_outputmix.outputMix   = outputMix;
    audioSink.pLocator            = (void*)&locator_outputmix;
    audioSink.pFormat             = NULL;

    /* Select the reverb preset */
    fprintf(stdout, "\nUsing preset ");
    switch(preset) {
        case SL_REVERBPRESET_NONE:
            fprintf(stdout, "SL_REVERBPRESET_NONE, don't expect to hear reverb\n");
            break;
        case SL_REVERBPRESET_SMALLROOM: fprintf(stdout, "SL_REVERBPRESET_SMALLROOM\n"); break;
        case SL_REVERBPRESET_MEDIUMROOM: fprintf(stdout, "SL_REVERBPRESET_MEDIUMROOM\n"); break;
        case SL_REVERBPRESET_LARGEROOM: fprintf(stdout, "SL_REVERBPRESET_LARGEROOM\n"); break;
        case SL_REVERBPRESET_MEDIUMHALL: fprintf(stdout, "SL_REVERBPRESET_MEDIUMHALL\n"); break;
        case SL_REVERBPRESET_LARGEHALL: fprintf(stdout, "SL_REVERBPRESET_LARGEHALL\n"); break;
        case SL_REVERBPRESET_PLATE: fprintf(stdout, "SL_REVERBPRESET_PLATE\n"); break;
        default:
            fprintf(stdout, "unknown, use at your own risk\n"); break;
    }
    result = (*reverbItf)->SetPreset(reverbItf, preset);
    ExitOnError(result);

    /* ------------------------------------------------------ */
    /* Configuration of the player  */

    /* Set arrays required[] and iidArray[] for required interfaces */
    /*  (SLPlayItf is implicit) */
    required[0] = SL_BOOLEAN_TRUE;
    iidArray[0] = SL_IID_PREFETCHSTATUS;
    required[1] = SL_BOOLEAN_TRUE;
    iidArray[1] = SL_IID_EFFECTSEND;

#ifdef ANDROID
    /* Setup the data source structure for the URI */
    locatorFd.locatorType = SL_DATALOCATOR_ANDROIDFD;
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        ExitOnError(SL_RESULT_RESOURCE_ERROR);
    }
    locatorFd.fd = (SLint32) fd;
    locatorFd.length = SL_DATALOCATOR_ANDROIDFD_USE_FILE_SIZE;
    locatorFd.offset = 0;
#else
    locatorUri.locatorType = SL_DATALOCATOR_URI;
    locatorUri.URI = (SLchar *) path;
#endif

    mime.formatType = SL_DATAFORMAT_MIME;
    /*     this is how ignored mime information is specified, according to OpenSL ES spec
     *     in 9.1.6 SLDataFormat_MIME and 8.23 SLMetadataTraversalItf GetChildInfo */
    mime.mimeType      = (SLchar*)NULL;
    mime.containerType = SL_CONTAINERTYPE_UNSPECIFIED;

    audioSource.pFormat  = (void*)&mime;
#ifdef ANDROID
    audioSource.pLocator = (void*)&locatorFd;
#else
    audioSource.pLocator = (void*)&locatorUri;
#endif

    /* Create the audio player */
    result = (*EngineItf)->CreateAudioPlayer(EngineItf, &player, &audioSource, &audioSink, 2,
            iidArray, required);
    ExitOnError(result);

    /* Realize the player in synchronous mode. */
    result = (*player)->Realize(player, SL_BOOLEAN_FALSE); ExitOnError(result);
    fprintf(stdout, "URI example: after Realize\n");

    /* Get the SLPlayItf, SLPrefetchStatusItf and SLEffectSendItf interfaces for the player*/
    result = (*player)->GetInterface(player, SL_IID_PLAY, (void*)&playItf);
    ExitOnError(result);

    result = (*player)->GetInterface(player, SL_IID_EFFECTSEND, (void*)&effectSendItf);
    ExitOnError(result);

    fprintf(stdout, "Player configured\n");

    /* ------------------------------------------------------ */
    /* Playback and test */

    /* Get duration */
    SLmillisecond durationInMsec = SL_TIME_UNKNOWN;
    result = (*playItf)->GetDuration(playItf, &durationInMsec);
    ExitOnError(result);
    if (durationInMsec == SL_TIME_UNKNOWN) {
        durationInMsec = 5000;
    }

    /* Feed the output mix' reverb from the audio player using the given send level */
    result = (*effectSendItf)->EnableEffectSend(effectSendItf, reverbItf, SL_BOOLEAN_TRUE,
            sendLevel);
    ExitOnError(result);

    result = (*effectSendItf)->SetDirectLevel(effectSendItf, directLevel);
    ExitOnError(result);
    fprintf(stdout, "Set direct level to %dmB\n", directLevel);

    result = (*effectSendItf)->SetSendLevel(effectSendItf, reverbItf, sendLevel);
    ExitOnError(result);
    fprintf(stdout, "Set send level to %dmB\n", sendLevel);

    /* Start playback */
    result = (*playItf)->SetPlayState( playItf, SL_PLAYSTATE_PLAYING );
    ExitOnError(result);

    /* Disable preset reverb every TIME_S_BETWEEN_SETTING_CHANGE seconds */
    SLboolean enabled = SL_BOOLEAN_TRUE;
    for(unsigned int j=0 ; j<(durationInMsec/(1000*TIME_S_BETWEEN_SETTING_CHANGE)) ; j++) {
        enabled = !enabled;
        result = (*reverbItf)->SetPreset(reverbItf, enabled ? preset : SL_REVERBPRESET_NONE);
        if (enabled) {
            fprintf(stdout, "Reverb on\n");
        } else {
            fprintf(stdout, "Reverb off\n");
        }
        ExitOnError(result);
        usleep(TIME_S_BETWEEN_SETTING_CHANGE * 1000 * 1000);
    }

    /* Make sure player is stopped */
    fprintf(stdout, "Stopping playback\n");
    result = (*playItf)->SetPlayState(playItf, SL_PLAYSTATE_STOPPED);
    ExitOnError(result);

    /* Destroy the player */
    (*player)->Destroy(player);

    /* Destroy Output Mix object */
    (*outputMix)->Destroy(outputMix);

#ifdef ANDROID
    close(fd);
#endif
}

//-----------------------------------------------------------------
int main(int argc, char* const argv[])
{
    SLresult    result;
    SLObjectItf sl;

    fprintf(stdout, "OpenSL ES test %s: exercises SLEffectSendItf ", argv[0]);
    fprintf(stdout, "on AudioPlayer and SLPresetReverbItf on OutputMix.\n");
    fprintf(stdout, "Plays the sound file designated by the given path, ");
    fprintf(stdout, "and sends a specified amount of energy to a global reverb\n");
    fprintf(stdout, "(sendLevel in mB), with a given direct level (in mB).\n");
    fprintf(stdout, "Every %d seconds, the reverb turned on and off.\n",
            TIME_S_BETWEEN_SETTING_CHANGE);

    if (argc < 5) {
        fprintf(stdout, "Usage: \t%s path preset directLevel sendLevel\n", argv[0]);
        fprintf(stdout, "Example: \"%s /sdcard/my.mp3 6 -2000 0\" \n", argv[0]);
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

    // intentionally not checking that levels are of correct value
    TestSendToPresetReverb(sl, argv[1], atoi(argv[2]), (SLmillibel)atoi(argv[3]),
            (SLmillibel)atoi(argv[4]));

    /* Shutdown OpenSL ES */
    (*sl)->Destroy(sl);

    return EXIT_SUCCESS;
}
