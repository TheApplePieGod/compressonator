//===============================================================================
// Copyright (c) 2007-2016  Advanced Micro Devices, Inc. All rights reserved.
// Copyright (c) 2004-2006 ATI Technologies Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//  File Name:   Compress.cpp
//  Description: A library to compress/decompress textures
//
//  Revisions
//  Apr 2014    -    Refactored Library
//                   Code clean to support MSV 2010 and up
//////////////////////////////////////////////////////////////////////////////
        
#include "Compressonator.h"
#include "Compress.h"
#include <tchar.h>
#include <assert.h>    

CodecType GetCodecType(CMP_FORMAT format)
{
    switch(format)
    {
        case CMP_FORMAT_ARGB_2101010:            return CT_None;
        case CMP_FORMAT_RGBA_8888:               return CT_None;
        case CMP_FORMAT_BGRA_8888:               return CT_None;
        case CMP_FORMAT_ARGB_8888:               return CT_None;
        case CMP_FORMAT_RGB_888:                 return CT_None;
        case CMP_FORMAT_RG_8:                    return CT_None;
        case CMP_FORMAT_R_8:                     return CT_None;
        case CMP_FORMAT_ARGB_16:                 return CT_None;
        case CMP_FORMAT_RG_16:                   return CT_None;
        case CMP_FORMAT_R_16:                    return CT_None;
        case CMP_FORMAT_ARGB_16F:                return CT_None;
        case CMP_FORMAT_RG_16F:                  return CT_None;
        case CMP_FORMAT_R_16F:                   return CT_None;
        case CMP_FORMAT_ARGB_32F:                return CT_None;
        case CMP_FORMAT_RG_32F:                  return CT_None;
        case CMP_FORMAT_R_32F:                   return CT_None;
        case CMP_FORMAT_RGBE_32F:                return CT_None;
#ifdef ARGB_32_SUPPORT
        case CMP_FORMAT_ARGB_32:                 return CT_None;
        case CMP_FORMAT_RG_32:                   return CT_None;
        case CMP_FORMAT_R_32:                    return CT_None;
#endif  // ARGB_32_SUPPORT
        case CMP_FORMAT_DXT1:                    return CT_DXT1;
        case CMP_FORMAT_DXT3:                    return CT_DXT3;
        case CMP_FORMAT_DXT5:                    return CT_DXT5;
        case CMP_FORMAT_DXT5_xGBR:               return CT_DXT5_xGBR;
        case CMP_FORMAT_DXT5_RxBG:               return CT_DXT5_RxBG;
        case CMP_FORMAT_DXT5_RBxG:               return CT_DXT5_RBxG;
        case CMP_FORMAT_DXT5_xRBG:               return CT_DXT5_xRBG;
        case CMP_FORMAT_DXT5_RGxB:               return CT_DXT5_RGxB;
        case CMP_FORMAT_DXT5_xGxR:               return CT_DXT5_xGxR;
        case CMP_FORMAT_ATI1N:                   return CT_ATI1N;
        case CMP_FORMAT_ATI2N:                   return CT_ATI2N;
        case CMP_FORMAT_ATI2N_XY:                return CT_ATI2N_XY;
        case CMP_FORMAT_ATI2N_DXT5:              return CT_ATI2N_DXT5;
        case CMP_FORMAT_BC1:                     return CT_DXT1;
        case CMP_FORMAT_BC2:                     return CT_DXT3;
        case CMP_FORMAT_BC3:                     return CT_DXT5;
        case CMP_FORMAT_BC4:                     return CT_ATI1N;
        case CMP_FORMAT_BC5:                     return CT_ATI2N_XY;
        case CMP_FORMAT_BC6H:                    return CT_BC6H; 
        case CMP_FORMAT_BC6H_SF:                 return CT_BC6H_SF;
        case CMP_FORMAT_BC7:                     return CT_BC7;    
        case CMP_FORMAT_ASTC:                    return CT_ASTC;    
        case CMP_FORMAT_ATC_RGB:                 return CT_ATC_RGB;
        case CMP_FORMAT_ATC_RGBA_Explicit:       return CT_ATC_RGBA_Explicit;
        case CMP_FORMAT_ATC_RGBA_Interpolated:   return CT_ATC_RGBA_Interpolated;
        case CMP_FORMAT_ETC_RGB:                 return CT_ETC_RGB;
        case CMP_FORMAT_ETC2_RGB:                return CT_ETC2_RGB;
        case CMP_FORMAT_GT:                      return CT_GT;
        default: assert(0);                            return CT_Unknown;
    }
}

CMP_ERROR GetError(CodecError err)
{
    switch(err)
    {
        case CE_OK: return CMP_OK;
        case CE_Aborted: return CMP_ABORTED;
        case CE_Unknown: return CMP_ERR_GENERIC;
        default: return CMP_ERR_GENERIC;
    }
}

CMP_ERROR CheckTexture(const CMP_Texture* pTexture, bool bSource)
{
    assert(pTexture);
    if(pTexture == NULL)
        return (bSource ? CMP_ERR_INVALID_SOURCE_TEXTURE : CMP_ERR_INVALID_DEST_TEXTURE);

    assert(pTexture->dwSize == sizeof(CMP_Texture));
    if(pTexture->dwSize != sizeof(CMP_Texture))
        return (bSource ? CMP_ERR_INVALID_SOURCE_TEXTURE : CMP_ERR_INVALID_DEST_TEXTURE);

    assert(pTexture->dwWidth > 0);
    if(pTexture->dwWidth <= 0 )
        return (bSource ? CMP_ERR_INVALID_SOURCE_TEXTURE : CMP_ERR_INVALID_DEST_TEXTURE);

    assert(pTexture->dwHeight > 0);
    if(pTexture->dwHeight <= 0 )
        return (bSource ? CMP_ERR_INVALID_SOURCE_TEXTURE : CMP_ERR_INVALID_DEST_TEXTURE);

    assert(pTexture->format >= CMP_FORMAT_ARGB_8888 && pTexture->format <= CMP_FORMAT_MAX);
    if(pTexture->format < CMP_FORMAT_ARGB_8888 || pTexture->format > CMP_FORMAT_MAX)
        return (bSource ? CMP_ERR_UNSUPPORTED_SOURCE_FORMAT : CMP_ERR_UNSUPPORTED_DEST_FORMAT);

    assert((pTexture->format != CMP_FORMAT_ARGB_8888 && pTexture->format != CMP_FORMAT_ARGB_2101010)
        || pTexture->dwPitch == 0 || pTexture->dwPitch >= (pTexture->dwWidth*4));
    if((pTexture->format == CMP_FORMAT_ARGB_8888 || pTexture->format == CMP_FORMAT_ARGB_2101010)
        && pTexture->dwPitch != 0 && pTexture->dwPitch < (pTexture->dwWidth*4))
        return (bSource ? CMP_ERR_UNSUPPORTED_SOURCE_FORMAT : CMP_ERR_UNSUPPORTED_DEST_FORMAT);

    assert(pTexture->pData);
    if(pTexture->pData == NULL)
        return (bSource ? CMP_ERR_INVALID_SOURCE_TEXTURE : CMP_ERR_INVALID_DEST_TEXTURE);

    CMP_DWORD dwDataSize = CMP_CalculateBufferSize(pTexture);
    assert(pTexture->dwDataSize >= dwDataSize);
    if(pTexture->dwDataSize < dwDataSize)
        return (bSource ? CMP_ERR_INVALID_SOURCE_TEXTURE : CMP_ERR_INVALID_DEST_TEXTURE);

    return CMP_OK;
}



CMP_ERROR CompressTexture(const CMP_Texture* pSourceTexture, CMP_Texture* pDestTexture, const CMP_CompressOptions* pOptions, CMP_Feedback_Proc pFeedbackProc, DWORD_PTR pUser1, DWORD_PTR pUser2, CodecType destType)
{
    // Compressing
    CCodec* pCodec = CreateCodec(destType);
    assert(pCodec);
    if(pCodec == NULL)
        return CMP_ERR_UNABLE_TO_INIT_CODEC;


    // Have we got valid options ?
    if(pOptions && pOptions->dwSize == sizeof(CMP_CompressOptions))
    {
        // Set weightings ?
        if(pOptions->bUseChannelWeighting && (pOptions->fWeightingRed > 0.0 || pOptions->fWeightingGreen > 0.0 || pOptions->fWeightingBlue > 0.0))
        {
            pCodec->SetParameter("UseChannelWeighting", (CMP_DWORD) 1);
            pCodec->SetParameter("WeightR",
                pOptions->fWeightingRed > MINIMUM_WEIGHT_VALUE ?
                (CODECFLOAT) pOptions->fWeightingRed : MINIMUM_WEIGHT_VALUE);
            pCodec->SetParameter("WeightG",
                pOptions->fWeightingGreen > MINIMUM_WEIGHT_VALUE ?
                (CODECFLOAT) pOptions->fWeightingGreen : MINIMUM_WEIGHT_VALUE);
            pCodec->SetParameter("WeightB",
                pOptions->fWeightingBlue > MINIMUM_WEIGHT_VALUE ?
                (CODECFLOAT) pOptions->fWeightingBlue : MINIMUM_WEIGHT_VALUE);
        }
        pCodec->SetParameter("UseAdaptiveWeighting", (CMP_DWORD) pOptions->bUseAdaptiveWeighting);
        pCodec->SetParameter("DXT1UseAlpha", (CMP_DWORD) pOptions->bDXT1UseAlpha);
        pCodec->SetParameter("AlphaThreshold", (CMP_DWORD) pOptions->nAlphaThreshold);
        // New override to that set quality if compresion for DXTn & ATInN codecs
        if (pOptions->fquality != AMD_CODEC_QUALITY_DEFAULT)
        {
#ifndef _WIN64
            if (pOptions->fquality < 0.3)
                pCodec->SetParameter("CompressionSpeed", (CMP_DWORD)CMP_Speed_SuperFast);
            else
                if (pOptions->fquality < 0.6)
                    pCodec->SetParameter("CompressionSpeed", (CMP_DWORD)CMP_Speed_Fast);
                else
#endif
                    pCodec->SetParameter("CompressionSpeed", (CMP_DWORD)CMP_Speed_Normal);
        }
        else
            pCodec->SetParameter("CompressionSpeed", (CMP_DWORD)pOptions->nCompressionSpeed);


        switch(destType)
        {
        case CT_BC7:
                pCodec->SetParameter("MultiThreading", (CMP_DWORD) !pOptions->bDisableMultiThreading);
                
                if (!pOptions->bDisableMultiThreading)
                    pCodec->SetParameter("NumThreads", (CMP_DWORD) pOptions->dwnumThreads);
                else
                    pCodec->SetParameter("NumThreads", (CMP_DWORD) 1);

                pCodec->SetParameter("ModeMask", (CMP_DWORD) pOptions->dwmodeMask);
                pCodec->SetParameter("ColourRestrict", (CMP_DWORD) pOptions->brestrictColour);
                pCodec->SetParameter("AlphaRestrict", (CMP_DWORD) pOptions->brestrictAlpha);
                pCodec->SetParameter("Quality", (CODECFLOAT) pOptions->fquality);
                break;
        case CT_ASTC:
                pCodec->SetParameter("Quality", (CODECFLOAT)pOptions->fquality);
                break;
        case CT_GT:
        case CT_BC6H:
        case CT_BC6H_SF:
                pCodec->SetParameter("Quality", (CODECFLOAT)pOptions->fquality);
#ifdef _DEBUG
                // napatel : remove this after
                // pCodec->SetParameter("NumThreads", (CMP_DWORD)1);
#endif
                break;
        }

        // This will eventually replace the above code for setting codec options
        if (pOptions->NumCmds > 0)
        {
            int maxCmds=pOptions->NumCmds;
            if (pOptions->NumCmds > AMD_MAX_CMDS) maxCmds = AMD_MAX_CMDS;
            for (int i=0; i<maxCmds; i++)
                pCodec->SetParameter(pOptions->CmdSet[i].strCommand, (CMP_CHAR*)pOptions->CmdSet[i].strParameter);
        }



    }

    CodecBufferType srcBufferType = GetCodecBufferType(pSourceTexture->format);

    CCodecBuffer* pSrcBuffer  = CreateCodecBuffer(srcBufferType, 
                                                  pSourceTexture->nBlockWidth, pSourceTexture->nBlockHeight, pSourceTexture->nBlockDepth,
                                                  pSourceTexture->dwWidth, pSourceTexture->dwHeight, pSourceTexture->dwPitch, pSourceTexture->pData);
    CCodecBuffer* pDestBuffer = pCodec->CreateBuffer(
                                                  pDestTexture->nBlockWidth, pDestTexture->nBlockHeight, pDestTexture->nBlockDepth,
                                                  pDestTexture->dwWidth, pDestTexture->dwHeight, pDestTexture->dwPitch, pDestTexture->pData);

    assert(pSrcBuffer);
    assert(pDestBuffer);
    if(pSrcBuffer == NULL || pDestBuffer == NULL)
    {
        SAFE_DELETE(pCodec);
        SAFE_DELETE(pSrcBuffer);
        SAFE_DELETE(pDestBuffer);
        return CMP_ERR_GENERIC;
    }

    DISABLE_FP_EXCEPTIONS;
    CodecError err = pCodec->Compress(*pSrcBuffer, *pDestBuffer, pFeedbackProc, pUser1, pUser2);
    RESTORE_FP_EXCEPTIONS;

    SAFE_DELETE(pCodec);
    SAFE_DELETE(pSrcBuffer);
    SAFE_DELETE(pDestBuffer);

    return GetError(err);
}

#ifdef THREADED_COMPRESS

class CATICompressThreadData
{
public:
    CATICompressThreadData();
    ~CATICompressThreadData();

    CCodec* m_pCodec;
    CCodecBuffer* m_pSrcBuffer;
    CCodecBuffer* m_pDestBuffer;
    CMP_Feedback_Proc m_pFeedbackProc;
    DWORD_PTR m_pUser1;
    DWORD_PTR m_pUser2;
};

CATICompressThreadData::CATICompressThreadData() : m_pCodec(NULL), m_pSrcBuffer(NULL), m_pDestBuffer(NULL), 
                                                   m_pFeedbackProc(NULL), m_pUser1(NULL), m_pUser2(NULL)
{
}

CATICompressThreadData::~CATICompressThreadData()
{
    SAFE_DELETE(m_pCodec);
    SAFE_DELETE(m_pSrcBuffer);
    SAFE_DELETE(m_pDestBuffer);
}

DWORD WINAPI ThreadedCompressProc(LPVOID lpParameter)
{
    CATICompressThreadData *pThreadData = (CATICompressThreadData*) lpParameter;
    DISABLE_FP_EXCEPTIONS;
    CodecError err = pThreadData->m_pCodec->Compress(*pThreadData->m_pSrcBuffer, *pThreadData->m_pDestBuffer, pThreadData->m_pFeedbackProc, pThreadData->m_pUser1, pThreadData->m_pUser2);
    RESTORE_FP_EXCEPTIONS;
    return err;
}

CMP_ERROR ThreadedCompressTexture(const CMP_Texture* pSourceTexture, CMP_Texture* pDestTexture, const CMP_CompressOptions* pOptions, CMP_Feedback_Proc pFeedbackProc, DWORD_PTR pUser1, DWORD_PTR pUser2, CodecType destType)
{
    // Note function should not be called for the following Codecs....
    if (destType == CT_BC7)  return CMP_ABORTED; 
    if (destType == CT_GT)   return CMP_ABORTED;
    if (destType == CT_ASTC) return CMP_ABORTED; 

    DWORD dwMaxThreadCount = min(f_dwProcessorCount, MAX_THREADS);
    DWORD dwLinesRemaining = pDestTexture->dwHeight;
    CMP_BYTE* pSourceData = pSourceTexture->pData;
    CMP_BYTE* pDestData = pDestTexture->pData;

    CATICompressThreadData aThreadData[MAX_THREADS];
    HANDLE ahThread[MAX_THREADS];

    DWORD dwThreadCount = 0;
    for(DWORD dwThread = 0; dwThread < dwMaxThreadCount; dwThread++)
    {
        CATICompressThreadData& threadData = aThreadData[dwThread];

        // Compressing
        threadData.m_pCodec = CreateCodec(destType);
        assert(threadData.m_pCodec);
        if(threadData.m_pCodec == NULL)
            return CMP_ERR_UNABLE_TO_INIT_CODEC;

        // Have we got valid options ?
        if(pOptions && pOptions->dwSize == sizeof(CMP_CompressOptions))
        {
            // Set weightings ?
            if(pOptions->bUseChannelWeighting && (pOptions->fWeightingRed > 0.0 || pOptions->fWeightingGreen > 0.0 || pOptions->fWeightingBlue > 0.0))
            {
                threadData.m_pCodec->SetParameter("UseChannelWeighting", (CMP_DWORD) 1);
                threadData.m_pCodec->SetParameter("WeightR",
                    pOptions->fWeightingRed > MINIMUM_WEIGHT_VALUE ?
                    (CODECFLOAT) pOptions->fWeightingRed : MINIMUM_WEIGHT_VALUE);
                threadData.m_pCodec->SetParameter("WeightG",
                    pOptions->fWeightingGreen > MINIMUM_WEIGHT_VALUE ?
                    (CODECFLOAT) pOptions->fWeightingGreen : MINIMUM_WEIGHT_VALUE);
                threadData.m_pCodec->SetParameter("WeightB",
                    pOptions->fWeightingBlue > MINIMUM_WEIGHT_VALUE ?
                    (CODECFLOAT) pOptions->fWeightingBlue : MINIMUM_WEIGHT_VALUE);
            }
            threadData.m_pCodec->SetParameter("UseAdaptiveWeighting", (CMP_DWORD) pOptions->bUseAdaptiveWeighting);
            threadData.m_pCodec->SetParameter("DXT1UseAlpha", (CMP_DWORD) pOptions->bDXT1UseAlpha);
            threadData.m_pCodec->SetParameter("AlphaThreshold", (CMP_DWORD) pOptions->nAlphaThreshold);

            // New override to that set quality if compresion for DXTn & ATInN codecs
            if (pOptions->fquality != AMD_CODEC_QUALITY_DEFAULT)
            {
                if (pOptions->fquality < 0.3)
                    threadData.m_pCodec->SetParameter("CompressionSpeed", (CMP_DWORD)CMP_Speed_SuperFast);
                else
                    if (pOptions->fquality < 0.6)
                        threadData.m_pCodec->SetParameter("CompressionSpeed", (CMP_DWORD)CMP_Speed_Fast);
                    else
                        threadData.m_pCodec->SetParameter("CompressionSpeed", (CMP_DWORD)CMP_Speed_Normal);
            }
            else
                threadData.m_pCodec->SetParameter("CompressionSpeed", (CMP_DWORD)pOptions->nCompressionSpeed);



            switch(destType)
            {
            case CT_BC6H: 
                    // Reserved 
                    break;
            }

            // This will eventually replace the above code for setting codec options
            // It is currently implemented with BC6H and can be expanded to other codec
            if (pOptions->NumCmds > 0)
            {
                int maxCmds = pOptions->NumCmds;
                if (pOptions->NumCmds > AMD_MAX_CMDS) maxCmds = AMD_MAX_CMDS;
                for (int i = 0; i<maxCmds; i++)
                    threadData.m_pCodec->SetParameter(pOptions->CmdSet[i].strCommand, (CMP_CHAR*)pOptions->CmdSet[i].strParameter);
            }


        }

        CodecBufferType srcBufferType = GetCodecBufferType(pSourceTexture->format);

        DWORD dwThreadsRemaining = dwMaxThreadCount - dwThread;
        DWORD dwHeight = 0;
        if(dwThreadsRemaining > 1)
        {
            DWORD dwBlockHeight = threadData.m_pCodec->GetBlockHeight();
            dwHeight = dwLinesRemaining / dwThreadsRemaining;
            dwHeight = min(((dwHeight + dwBlockHeight - 1) / dwBlockHeight) * dwBlockHeight, dwLinesRemaining); // Round by block height
            dwLinesRemaining -= dwHeight;
        }
        else
            dwHeight = dwLinesRemaining;

        if(dwHeight > 0)
        {
            threadData.m_pSrcBuffer = CreateCodecBuffer(srcBufferType, 
                                                        pSourceTexture->nBlockWidth, pSourceTexture->nBlockHeight, pSourceTexture->nBlockDepth,
                                                        pSourceTexture->dwWidth, dwHeight, pSourceTexture->dwPitch, pSourceData);
            threadData.m_pDestBuffer = threadData.m_pCodec->CreateBuffer(
                                                        pDestTexture->nBlockWidth, pDestTexture->nBlockHeight, pDestTexture->nBlockDepth,
                                                        pDestTexture->dwWidth, dwHeight, pDestTexture->dwPitch, pDestData);

            pSourceData += CalcBufferSize(pSourceTexture->format, pSourceTexture->dwWidth, dwHeight, pSourceTexture->dwPitch, pSourceTexture->nBlockWidth, pSourceTexture->nBlockHeight);
            pDestData += CalcBufferSize(destType, pDestTexture->dwWidth, dwHeight, pDestTexture->nBlockWidth, pDestTexture->nBlockHeight);

            assert(threadData.m_pSrcBuffer);
            assert(threadData.m_pDestBuffer);
            if(threadData.m_pSrcBuffer == NULL || threadData.m_pDestBuffer == NULL)
                return CMP_ERR_GENERIC;

            threadData.m_pFeedbackProc = pFeedbackProc;
            threadData.m_pUser1 = pUser1;
            threadData.m_pUser2 = pUser2;

            DWORD dwThreadID;
            ahThread[dwThreadCount++] = CreateThread(NULL, 0, ThreadedCompressProc, &threadData, 0, &dwThreadID);
        }
    }

    WaitForMultipleObjects(dwThreadCount, ahThread, true, INFINITE);

    CodecError err = CE_OK;
    for(DWORD dwThread = 0; dwThread < dwThreadCount; dwThread++)
    {
        DWORD dwExitCode;

        if(err == CE_OK && GetExitCodeThread(ahThread[dwThread], &dwExitCode))
            err = (CodecError) dwExitCode;

        CloseHandle(ahThread[dwThread]);
    }

    return GetError(err);
}
#endif // THREADED_COMPRESS

