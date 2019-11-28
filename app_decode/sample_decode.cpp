/******************************************************************************\
Copyright (c) 2005-2018, Intel Corporation
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

This sample was distributed or derived from the Intel's Media Samples package.
The original version of this sample may be obtained from https://software.intel.com/en-us/intel-media-server-studio
or https://software.intel.com/en-us/media-client-solutions-support.
\**********************************************************************************/

#include "mfx_samples_config.h"

#include "pipeline_decode.h"
#include <sstream>
#include "version.h"
#include "ConfigFile.h"

#ifndef MFX_VERSION
#error MFX_VERSION not defined
#endif

void PrintHelp(msdk_char *strAppName, const msdk_char *strErrorMessage)
{
    msdk_printf(MSDK_STRING("Decoding Sample Version %s\n\n"), GetMSDKSampleVersion().c_str());

    if (strErrorMessage)
    {
        msdk_printf(MSDK_STRING("Error: %s\n"), strErrorMessage);
    }

    msdk_printf(MSDK_STRING("Usage: %s <codecid> [<options>] -i InputBitstream\n"), strAppName);
    msdk_printf(MSDK_STRING("   or: %s <codecid> [<options>] -i InputBitstream -r\n"), strAppName);
    msdk_printf(MSDK_STRING("   or: %s <codecid> [<options>] -i InputBitstream -o OutputYUVFile\n"), strAppName);
    msdk_printf(MSDK_STRING("\n"));
    msdk_printf(MSDK_STRING("Supported codecs (<codecid>):\n"));
    msdk_printf(MSDK_STRING("   <codecid>=h264|mpeg2|vc1|mvc|jpeg|vp9 - built-in Media SDK codecs\n"));
    msdk_printf(MSDK_STRING("   <codecid>=h265|vp9|capture            - in-box Media SDK plugins (may require separate downloading and installation)\n"));
    msdk_printf(MSDK_STRING("\n"));
    msdk_printf(MSDK_STRING("Work models:\n"));
    msdk_printf(MSDK_STRING("  1. Performance model: decoding on MAX speed, no rendering, no YUV dumping (no -r or -o option)\n"));
    msdk_printf(MSDK_STRING("  2. Rendering model: decoding with rendering on the screen (-r option)\n"));
    msdk_printf(MSDK_STRING("  3. Dump model: decoding with YUV dumping (-o option)\n"));
    msdk_printf(MSDK_STRING("\n"));
    msdk_printf(MSDK_STRING("Options:\n"));
    msdk_printf(MSDK_STRING("   [-?]                      - print help\n"));
    msdk_printf(MSDK_STRING("   [-hw]                     - use platform specific SDK implementation (default)\n"));
    msdk_printf(MSDK_STRING("   [-sw]                     - use software implementation, if not specified platform specific SDK implementation is used\n"));
    msdk_printf(MSDK_STRING("   [-p plugin]               - decoder plugin. Supported values: hevcd_sw, hevcd_hw, vp8d_hw, vp9d_hw, camera_hw, capture_hw\n"));
    msdk_printf(MSDK_STRING("   [-path path]              - path to plugin (valid only in pair with -p option)\n"));
    msdk_printf(MSDK_STRING("                               (optional for Media SDK in-box plugins, required for user-decoder ones)\n"));
    msdk_printf(MSDK_STRING("   [-f]                      - rendering framerate\n"));
    msdk_printf(MSDK_STRING("   [-w]                      - output width\n"));
    msdk_printf(MSDK_STRING("   [-h]                      - output height\n"));
    msdk_printf(MSDK_STRING("   [-di bob/adi]             - enable deinterlacing BOB/ADI\n"));
#if (MFX_VERSION >= 1025)
    msdk_printf(MSDK_STRING("   [-d]                      - enable decode error report\n"));
#endif
    msdk_printf(MSDK_STRING("\n"));
    msdk_printf(MSDK_STRING("JPEG Chroma Type:\n"));
    msdk_printf(MSDK_STRING("   [-jpeg_rgb] - RGB Chroma Type\n"));
    msdk_printf(MSDK_STRING("Output format parameters:\n"));
    msdk_printf(MSDK_STRING("   [-i420] - pipeline output format: NV12, output file format: I420\n"));
    msdk_printf(MSDK_STRING("   [-nv12] - pipeline output format: NV12, output file format: NV12\n"));
    msdk_printf(MSDK_STRING("   [-rgb4] - pipeline output format: RGB4, output file format: RGB4\n"));
    msdk_printf(MSDK_STRING("   [-rgb4_fcr] - pipeline output format: RGB4 in full color range, output file format: RGB4 in full color range\n"));
    msdk_printf(MSDK_STRING("   [-p010] - pipeline output format: P010, output file format: P010\n"));
    msdk_printf(MSDK_STRING("   [-a2rgb10] - pipeline output format: A2RGB10, output file format: A2RGB10\n"));
    msdk_printf(MSDK_STRING("\n"));
#if D3D_SURFACES_SUPPORT
    msdk_printf(MSDK_STRING("   [-d3d]                    - work with d3d9 surfaces\n"));
    msdk_printf(MSDK_STRING("   [-d3d11]                  - work with d3d11 surfaces\n"));
    msdk_printf(MSDK_STRING("   [-r]                      - render decoded data in a separate window \n"));
    msdk_printf(MSDK_STRING("   [-wall w h n m t tmo]     - same as -r, and positioned rendering window in a particular cell on specific monitor \n"));
    msdk_printf(MSDK_STRING("       w                     - number of columns of video windows on selected monitor\n"));
    msdk_printf(MSDK_STRING("       h                     - number of rows of video windows on selected monitor\n"));
    msdk_printf(MSDK_STRING("       n(0,.,w*h-1)          - order of video window in table that will be rendered\n"));
    msdk_printf(MSDK_STRING("       m(0,1..)              - monitor id \n"));
    msdk_printf(MSDK_STRING("       t(0/1)                - enable/disable window's title\n"));
    msdk_printf(MSDK_STRING("       tmo                   - timeout for -wall option\n"));
    msdk_printf(MSDK_STRING("\n"));

#endif
#if defined(LIBVA_SUPPORT)
    msdk_printf(MSDK_STRING("   [-vaapi]                  - work with vaapi surfaces\n"));
#endif
#if defined(LIBVA_X11_SUPPORT)
    msdk_printf(MSDK_STRING("   [-r]                      - render decoded data in a separate X11 window \n"));
#endif
#if defined(LIBVA_WAYLAND_SUPPORT)
    msdk_printf(MSDK_STRING("   [-rwld]                   - render decoded data in a Wayland window \n"));
    msdk_printf(MSDK_STRING("   [-perf]                   - turn on asynchronous flipping for Wayland rendering \n"));
#endif
#if defined(LIBVA_DRM_SUPPORT)
    msdk_printf(MSDK_STRING("   [-rdrm]                   - render decoded data in a thru DRM frame buffer\n"));
    msdk_printf(MSDK_STRING("   [-window x y w h]         - set render window position and size\n"));
#endif
    msdk_printf(MSDK_STRING("   [-low_latency]            - configures decoder for low latency mode (supported only for H.264 and JPEG codec)\n"));
    msdk_printf(MSDK_STRING("   [-calc_latency]           - calculates latency during decoding and prints log (supported only for H.264 and JPEG codec)\n"));
    msdk_printf(MSDK_STRING("   [-async]                  - depth of asynchronous pipeline. default value is 4. must be between 1 and 20\n"));
    msdk_printf(MSDK_STRING("   [-gpucopy::<on,off>] Enable or disable GPU copy mode\n"));
    msdk_printf(MSDK_STRING("   [-robust:soft]            - GPU hang recovery by inserting an IDR frame\n"));
    msdk_printf(MSDK_STRING("   [-timeout]                - timeout in seconds\n"));
#if MFX_VERSION >= 1022
    msdk_printf(MSDK_STRING("   [-dec_postproc force/auto] - resize after decoder using direct pipe\n"));
    msdk_printf(MSDK_STRING("                  force: instruct to use decoder-based post processing\n"));
    msdk_printf(MSDK_STRING("                         or fail if the decoded stream is unsupported\n"));
    msdk_printf(MSDK_STRING("                  auto: instruct to use decoder-based post processing for supported streams \n"));
    msdk_printf(MSDK_STRING("                        or perform VPP operation through separate pipeline component for unsupported streams\n"));

#endif //MFX_VERSION >= 1022
#if !defined(_WIN32) && !defined(_WIN64)
    msdk_printf(MSDK_STRING("   [-threads_num]            - number of mediasdk task threads\n"));
    msdk_printf(MSDK_STRING("   [-threads_schedtype]      - scheduling type of mediasdk task threads\n"));
    msdk_printf(MSDK_STRING("   [-threads_priority]       - priority of mediasdk task threads\n"));
    msdk_printf(MSDK_STRING("\n"));
    msdk_thread_printf_scheduling_help();
#endif
#if defined(_WIN32) || defined(_WIN64)
    msdk_printf(MSDK_STRING("   [-jpeg_rotate n]          - rotate jpeg frame n degrees \n"));
    msdk_printf(MSDK_STRING("       n(90,180,270)         - number of degrees \n"));

    msdk_printf(MSDK_STRING("\nFeatures: \n"));
    msdk_printf(MSDK_STRING("   Press 1 to toggle fullscreen rendering on/off\n"));
#endif
    msdk_printf(MSDK_STRING("\n"));
    msdk_printf(MSDK_STRING("Example:\n"));
    msdk_printf(MSDK_STRING("  %s h265 -i in.bit -o out.yuv -p 15dd936825ad475ea34e35f3f54217a6\n"), strAppName);
}

mfxStatus ParseInputString(sInputParams* pParams)
{
    MSDK_CHECK_POINTER(pParams, MFX_ERR_NULL_PTR);

#if 1
	msdk_printf(MSDK_STRING("[DEBUG]ParseInputString ----------------------------------------------IN\n"));
	Config config("decode.cfg");
	std::string setting;
	printf("InputFile : %s\n", config.Read<std::string>("InputFile").c_str());
	printf("OutputFile : %s\n", config.Read<std::string>("OutputFile").c_str());

	printf("EncoderMode : %d\n", config.Read<int>("EncoderMode"));
	printf("CodecType : %s\n", config.Read<std::string>("CodecType").c_str());
#endif

    // set default implementation
    pParams->bUseHWLib = true;
    pParams->bUseFullColorRange = false;
#if defined(LIBVA_SUPPORT)
    pParams->libvaBackend = MFX_LIBVA_DRM;
#endif

	wchar_t ws[256];
	swprintf(ws, 256, L"%hs", config.Read<std::string>("CodecType").c_str());
	mfxStatus sts = StrFormatToCodecFormatFourCC(ws, pParams->videoType);

	if (sts != MFX_ERR_NONE)
	{
		msdk_printf(MSDK_STRING("[DEBUG]Unknown codec %s\n"), config.Read<std::string>("CodecType").c_str());
		return MFX_ERR_UNSUPPORTED;
	}
	if (!IsEncodeCodecSupported(pParams->videoType))
	{
		msdk_printf(MSDK_STRING("[DEBUG]Unsupported codec %s\n"), config.Read<std::string>("CodecType").c_str());
		return MFX_ERR_UNSUPPORTED;
	}
	if (pParams->videoType == CODEC_MVC)
	{
		pParams->videoType = MFX_CODEC_AVC;
		pParams->bIsMVC = true;
	}

	if (0 == config.Read<int>("EncoderMode"))
	{
		pParams->bUseHWLib = true;
	}
	else if (1 == config.Read<int>("EncoderMode"))
	{
		pParams->bUseHWLib = false;
	}

	if (!config.Read<std::string>("InputFile").empty())
	{
		memset(pParams->strSrcFile,0x0, sizeof(wchar_t)*MSDK_MAX_FILENAME_LEN);
		swprintf(pParams->strSrcFile, MSDK_MAX_FILENAME_LEN, L"%hs", config.Read<std::string>("InputFile").c_str());
		//swprintf(pParams->strSrcFile, MSDK_MAX_FILENAME_LEN, L"%hs", ".\\enc.h265");
	}

	if (!config.Read<std::string>("OutputFile").empty())
	{
		pParams->mode = MODE_FILE_DUMP;
		memset(pParams->strDstFile, 0x0, sizeof(wchar_t) * MSDK_MAX_FILENAME_LEN);
		swprintf(pParams->strDstFile, 256, L"%hs", config.Read<std::string>("OutputFile").c_str());
		//swprintf(pParams->strDstFile, MSDK_MAX_FILENAME_LEN, L"%hs", ".\\enc.yuv");
	}

    if (0 == msdk_strlen(pParams->strSrcFile))
    {
        msdk_printf(MSDK_STRING("error: source file name not found"));
        return MFX_ERR_UNSUPPORTED;
    }

    if ((pParams->mode == MODE_FILE_DUMP) && (0 == msdk_strlen(pParams->strDstFile)))
    {
        msdk_printf(MSDK_STRING("error: destination file name not found"));
        return MFX_ERR_UNSUPPORTED;
    }

    if (MFX_CODEC_MPEG2   != pParams->videoType &&
        MFX_CODEC_AVC     != pParams->videoType &&
        MFX_CODEC_HEVC    != pParams->videoType &&
        MFX_CODEC_VC1     != pParams->videoType &&
        MFX_CODEC_JPEG    != pParams->videoType &&
        MFX_CODEC_VP8     != pParams->videoType &&
        MFX_CODEC_VP9     != pParams->videoType)
    {
        PrintHelp(L"Demo", MSDK_STRING("Unknown codec"));
        return MFX_ERR_UNSUPPORTED;
    }

    if (pParams->nAsyncDepth == 0)
    {
        pParams->nAsyncDepth = 4; //set by default;
    }

    return MFX_ERR_NONE;
}

#if defined(_WIN32) || defined(_WIN64)
int _tmain(int argc, TCHAR *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    sInputParams        Params;   // input parameters from command line
    CDecodingPipeline   Pipeline; // pipeline for decoding, includes input file reader, decoder and output file writer

    mfxStatus sts = MFX_ERR_NONE; // return value check

    sts = ParseInputString(&Params);
    if (sts == MFX_ERR_ABORTED)
    {
        // No error, just need to close app normally
        return MFX_ERR_NONE;
    }
    MSDK_CHECK_PARSE_RESULT(sts, MFX_ERR_NONE, 1);

	printf("[debug][main]--------------------src[wchar_t]=%ls\r\n", Params.strSrcFile);
	printf("[debug][main]--------------------dst[wchar_t]=%ls\r\n", Params.strDstFile);

    if (Params.bIsMVC)
        Pipeline.SetMultiView();

    sts = Pipeline.Init(&Params);
    MSDK_CHECK_STATUS(sts, "Pipeline.Init failed");

    // print stream info
    Pipeline.PrintInfo();

    msdk_printf(MSDK_STRING("Decoding started\n"));

    mfxU64 prevResetBytesCount = 0xFFFFFFFFFFFFFFFF;
    for (;;)
    {
        sts = Pipeline.RunDecoding();

        if (MFX_ERR_INCOMPATIBLE_VIDEO_PARAM == sts || MFX_ERR_DEVICE_LOST == sts || MFX_ERR_DEVICE_FAILED == sts)
        {
            if (prevResetBytesCount == Pipeline.GetTotalBytesProcessed())
            {
                msdk_printf(MSDK_STRING("\nERROR: No input data was consumed since last reset. Quitting to avoid looping forever.\n"));
                break;
            }
            prevResetBytesCount = Pipeline.GetTotalBytesProcessed();


            if (MFX_ERR_INCOMPATIBLE_VIDEO_PARAM == sts)
            {
                msdk_printf(MSDK_STRING("\nERROR: Incompatible video parameters detected. Recovering...\n"));
            }
            else
            {
                msdk_printf(MSDK_STRING("\nERROR: Hardware device was lost or returned unexpected error. Recovering...\n"));
                sts = Pipeline.ResetDevice();
                MSDK_CHECK_STATUS(sts, "Pipeline.ResetDevice failed");
            }

            sts = Pipeline.ResetDecoder(&Params);
            MSDK_CHECK_STATUS(sts, "Pipeline.ResetDecoder failed");
            continue;
        }
        else
        {
            MSDK_CHECK_STATUS(sts, "Pipeline.RunDecoding failed");
            break;
        }
    }

    msdk_printf(MSDK_STRING("\nDecoding finished\n"));

    return 0;
}
