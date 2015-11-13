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

/* 3DMacroscopic implementation */

#include "sles_allinclusive.h"


static SLresult I3DMacroscopic_SetSize(SL3DMacroscopicItf self,
    SLmillimeter width, SLmillimeter height, SLmillimeter depth)
{
    SL_ENTER_INTERFACE

    if (!((0 <= width) && (width <= SL_MILLIMETER_MAX) &&
        (0 <= height) && (height <= SL_MILLIMETER_MAX) &&
        (0 <= depth) && (depth <= SL_MILLIMETER_MAX))) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        I3DMacroscopic *this = (I3DMacroscopic *) self;
        interface_lock_exclusive(this);
        this->mSize.mWidth = width;
        this->mSize.mHeight = height;
        this->mSize.mDepth = depth;
        interface_unlock_exclusive(this);
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult I3DMacroscopic_GetSize(SL3DMacroscopicItf self,
    SLmillimeter *pWidth, SLmillimeter *pHeight, SLmillimeter *pDepth)
{
    SL_ENTER_INTERFACE

    if (NULL == pWidth || NULL == pHeight || NULL == pDepth) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        I3DMacroscopic *this = (I3DMacroscopic *) self;
        interface_lock_shared(this);
        SLmillimeter width = this->mSize.mWidth;
        SLmillimeter height = this->mSize.mHeight;
        SLmillimeter depth = this->mSize.mDepth;
        interface_unlock_shared(this);
        *pWidth = width;
        *pHeight = height;
        *pDepth = depth;
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult I3DMacroscopic_SetOrientationAngles(SL3DMacroscopicItf self,
    SLmillidegree heading, SLmillidegree pitch, SLmillidegree roll)
{
    SL_ENTER_INTERFACE

    if (!((-360000 <= heading) && (heading <= 360000) &&
        (-90000 <= pitch) && (pitch <= 90000) &&
        (-360000 <= roll) && (roll <= 360000))) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        I3DMacroscopic *this = (I3DMacroscopic *) self;
        interface_lock_exclusive(this);
        this->mOrientationAngles.mHeading = heading;
        this->mOrientationAngles.mPitch = pitch;
        this->mOrientationAngles.mRoll = roll;
        this->mOrientationActive = ANGLES_SET_VECTORS_UNKNOWN;
        this->mRotatePending = SL_BOOLEAN_FALSE;
        // ++this->mGeneration;
        interface_unlock_exclusive(this);
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult I3DMacroscopic_SetOrientationVectors(SL3DMacroscopicItf self,
    const SLVec3D *pFront, const SLVec3D *pAbove)
{
    SL_ENTER_INTERFACE

    if (NULL == pFront || NULL == pAbove) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        I3DMacroscopic *this = (I3DMacroscopic *) self;
        SLVec3D front = *pFront;
        SLVec3D above = *pAbove;
        // NTH Check for vectors close to zero or close to parallel
        interface_lock_exclusive(this);
        this->mOrientationVectors.mFront = front;
        this->mOrientationVectors.mAbove = above;
        this->mOrientationVectors.mUp = above; // wrong
        this->mOrientationActive = ANGLES_UNKNOWN_VECTORS_SET;
        this->mRotatePending = SL_BOOLEAN_FALSE;
        interface_unlock_exclusive(this);
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult I3DMacroscopic_Rotate(SL3DMacroscopicItf self,
    SLmillidegree theta, const SLVec3D *pAxis)
{
    SL_ENTER_INTERFACE

    if (!((-360000 <= theta) && (theta <= 360000)) || NULL == pAxis) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        SLVec3D axis = *pAxis;
        // NTH Check that axis is not (close to) zero vector, length does not matter
        I3DMacroscopic *this = (I3DMacroscopic *) self;
        interface_lock_exclusive(this);
        while (this->mRotatePending)
            interface_cond_wait(this);
        this->mTheta = theta;
        this->mAxis = axis;
        this->mRotatePending = SL_BOOLEAN_TRUE;
        interface_unlock_exclusive(this);
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult I3DMacroscopic_GetOrientationVectors(SL3DMacroscopicItf self,
    SLVec3D *pFront, SLVec3D *pUp)
{
    SL_ENTER_INTERFACE

    if (NULL == pFront || NULL == pUp) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        I3DMacroscopic *this = (I3DMacroscopic *) self;
        interface_lock_exclusive(this);
        for (;;) {
            enum AnglesVectorsActive orientationActive = this->mOrientationActive;
            switch (orientationActive) {
            case ANGLES_COMPUTED_VECTORS_SET:    // not in 1.0.1
            case ANGLES_REQUESTED_VECTORS_SET:   // not in 1.0.1
            case ANGLES_UNKNOWN_VECTORS_SET:
            case ANGLES_SET_VECTORS_COMPUTED:
                {
                SLVec3D front = this->mOrientationVectors.mFront;
                SLVec3D up = this->mOrientationVectors.mUp;
                interface_unlock_exclusive(this);
                *pFront = front;
                *pUp = up;
                }
                break;
            case ANGLES_SET_VECTORS_UNKNOWN:
                this->mOrientationActive = ANGLES_SET_VECTORS_REQUESTED;
                // fall through
            case ANGLES_SET_VECTORS_REQUESTED:
                // matched by cond_broadcast in case multiple requesters
#if 0
                interface_cond_wait(this);
#else
                this->mOrientationActive = ANGLES_SET_VECTORS_COMPUTED;
#endif
                continue;
            default:
                interface_unlock_exclusive(this);
                assert(SL_BOOLEAN_FALSE);
                pFront->x = 0;
                pFront->y = 0;
                pFront->z = 0;
                pUp->x = 0;
                pUp->y = 0;
                pUp->z = 0;
                break;
            }
            break;
        }
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static const struct SL3DMacroscopicItf_ I3DMacroscopic_Itf = {
    I3DMacroscopic_SetSize,
    I3DMacroscopic_GetSize,
    I3DMacroscopic_SetOrientationAngles,
    I3DMacroscopic_SetOrientationVectors,
    I3DMacroscopic_Rotate,
    I3DMacroscopic_GetOrientationVectors
};

void I3DMacroscopic_init(void *self)
{
    I3DMacroscopic *this = (I3DMacroscopic *) self;
    this->mItf = &I3DMacroscopic_Itf;
    this->mSize.mWidth = 0;
    this->mSize.mHeight = 0;
    this->mSize.mDepth = 0;
    this->mOrientationAngles.mHeading = 0;
    this->mOrientationAngles.mPitch = 0;
    this->mOrientationAngles.mRoll = 0;
    memset(&this->mOrientationVectors, 0x55, sizeof(this->mOrientationVectors));
    this->mOrientationVectors.mFront.x = 0;
    this->mOrientationVectors.mFront.y = 0;
    this->mOrientationVectors.mFront.z = -1000;
    this->mOrientationVectors.mUp.x = 0;
    this->mOrientationVectors.mUp.y = 1000;
    this->mOrientationVectors.mUp.z = 0;
    this->mOrientationVectors.mAbove.x = 0;
    this->mOrientationVectors.mAbove.y = 0;
    this->mOrientationVectors.mAbove.z = 0;
    this->mOrientationActive = ANGLES_SET_VECTORS_COMPUTED;
    this->mTheta = 0x55555555;
    this->mAxis.x = 0x55555555;
    this->mAxis.y = 0x55555555;
    this->mAxis.z = 0x55555555;
    this->mRotatePending = SL_BOOLEAN_FALSE;
}
