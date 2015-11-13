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

/* MetadataExtraction implementation */

#include "sles_allinclusive.h"


static SLresult IMetadataExtraction_GetItemCount(SLMetadataExtractionItf self, SLuint32 *pItemCount)
{
    SL_ENTER_INTERFACE

    //IMetadataExtraction *this = (IMetadataExtraction *) self;
    if (NULL == pItemCount) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        *pItemCount = 0;
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IMetadataExtraction_GetKeySize(SLMetadataExtractionItf self,
    SLuint32 index, SLuint32 *pKeySize)
{
    SL_ENTER_INTERFACE

    //IMetadataExtraction *this = (IMetadataExtraction *) self;
    if (NULL == pKeySize) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        *pKeySize = 0;
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IMetadataExtraction_GetKey(SLMetadataExtractionItf self,
    SLuint32 index, SLuint32 keySize, SLMetadataInfo *pKey)
{
    SL_ENTER_INTERFACE

    //IMetadataExtraction *this = (IMetadataExtraction *) self;
    if (NULL == pKey) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        SLMetadataInfo key;
        key.size = 1;
        key.encoding = SL_CHARACTERENCODING_UTF8;
        memcpy((char *) key.langCountry, "en", 3);
        key.data[0] = 0;
        *pKey = key;
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IMetadataExtraction_GetValueSize(SLMetadataExtractionItf self,
    SLuint32 index, SLuint32 *pValueSize)
{
    SL_ENTER_INTERFACE

    //IMetadataExtraction *this = (IMetadataExtraction *) self;
    if (NULL == pValueSize) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        *pValueSize = 0;
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IMetadataExtraction_GetValue(SLMetadataExtractionItf self,
    SLuint32 index, SLuint32 valueSize, SLMetadataInfo *pValue)
{
    SL_ENTER_INTERFACE

    //IMetadataExtraction *this = (IMetadataExtraction *) self;
    if (NULL == pValue) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        SLMetadataInfo value;
        value.size = 1;
        value.encoding = SL_CHARACTERENCODING_UTF8;
        memcpy((char *) value.langCountry, "en", 3);
        value.data[0] = 0;
        *pValue = value;;
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IMetadataExtraction_AddKeyFilter(SLMetadataExtractionItf self,
    SLuint32 keySize, const void *pKey, SLuint32 keyEncoding,
    const SLchar *pValueLangCountry, SLuint32 valueEncoding, SLuint8 filterMask)
{
    SL_ENTER_INTERFACE

    if (NULL == pKey || NULL == pValueLangCountry || (filterMask & ~(SL_METADATA_FILTER_KEY |
        SL_METADATA_FILTER_KEY | SL_METADATA_FILTER_KEY))) {
        result = SL_RESULT_PARAMETER_INVALID;
    } else {
        IMetadataExtraction *this = (IMetadataExtraction *) self;
        interface_lock_exclusive(this);
        this->mKeySize = keySize;
        this->mKey = pKey;
        this->mKeyEncoding = keyEncoding;
        this->mValueLangCountry = pValueLangCountry; // should make a local copy
        this->mValueEncoding = valueEncoding;
        this->mFilterMask = filterMask;
        interface_unlock_exclusive(this);
        result = SL_RESULT_SUCCESS;
    }

    SL_LEAVE_INTERFACE
}


static SLresult IMetadataExtraction_ClearKeyFilter(SLMetadataExtractionItf self)
{
    SL_ENTER_INTERFACE

    IMetadataExtraction *this = (IMetadataExtraction *) self;
    this->mKeyFilter = 0;
    result = SL_RESULT_SUCCESS;

    SL_LEAVE_INTERFACE
}


static const struct SLMetadataExtractionItf_ IMetadataExtraction_Itf = {
    IMetadataExtraction_GetItemCount,
    IMetadataExtraction_GetKeySize,
    IMetadataExtraction_GetKey,
    IMetadataExtraction_GetValueSize,
    IMetadataExtraction_GetValue,
    IMetadataExtraction_AddKeyFilter,
    IMetadataExtraction_ClearKeyFilter
};

void IMetadataExtraction_init(void *self)
{
    IMetadataExtraction *this = (IMetadataExtraction *) self;
    this->mItf = &IMetadataExtraction_Itf;
    this->mKeySize = 0;
    this->mKey = NULL;
    this->mKeyEncoding = 0 /*TBD*/;
    this->mValueLangCountry = 0 /*TBD*/;
    this->mValueEncoding = 0 /*TBD*/;
    this->mFilterMask = 0 /*TBD*/;
    this->mKeyFilter = 0;
}
