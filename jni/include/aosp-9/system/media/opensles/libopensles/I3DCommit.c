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

/* 3DCommit implementation */

#include "sles_allinclusive.h"


static SLresult I3DCommit_Commit(SL3DCommitItf self)
{
    SL_ENTER_INTERFACE

    I3DCommit *this = (I3DCommit *) self;
    IObject *thisObject = InterfaceToIObject(this);
    object_lock_exclusive(thisObject);
    if (this->mDeferred) {
        SLuint32 myGeneration = this->mGeneration;
        do {
            ++this->mWaiting;
            object_cond_wait(thisObject);
        } while (this->mGeneration == myGeneration);
    }
    object_unlock_exclusive(thisObject);
    result = SL_RESULT_SUCCESS;

    SL_LEAVE_INTERFACE
}


static SLresult I3DCommit_SetDeferred(SL3DCommitItf self, SLboolean deferred)
{
    SL_ENTER_INTERFACE

    I3DCommit *this = (I3DCommit *) self;
    IObject *thisObject = InterfaceToIObject(this);
    object_lock_exclusive(thisObject);
    this->mDeferred = SL_BOOLEAN_FALSE != deferred; // normalize
    object_unlock_exclusive(thisObject);
    result = SL_RESULT_SUCCESS;

    SL_LEAVE_INTERFACE
}


static const struct SL3DCommitItf_ I3DCommit_Itf = {
    I3DCommit_Commit,
    I3DCommit_SetDeferred
};

void I3DCommit_init(void *self)
{
    I3DCommit *this = (I3DCommit *) self;
    this->mItf = &I3DCommit_Itf;
    this->mDeferred = SL_BOOLEAN_FALSE;
    this->mGeneration = 0;
    this->mWaiting = 0;
}
