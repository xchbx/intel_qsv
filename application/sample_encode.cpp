/******************************************************************************\
Copyright (c) 2005-2019, Intel Corporation
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

#include <memory>
#include "pipeline_encode.h"
#include "pipeline_user.h"
#include "pipeline_region_encode.h"
#include <stdarg.h>
#include <string>
#include "version.h"
#include "ConfigFile.h"

#define VAL_CHECK(val, argIdx, argName) \
{ \
    if (val) \
    { \
    PrintHelp(NULL, MSDK_STRING("Input argument number %d \"%s\" require more parameters"), argIdx, argName); \
    return MFX_ERR_UNSUPPORTED;\
    } \
}

#ifdef MOD_ENC
// Extensions for internal use, normally these macros are blank
#include "extension_macros.h"
#else
#define MOD_ENC_CREATE_PIPELINE
#define MOD_ENC_PRINT_HELP
#define MOD_ENC_PARSE_INPUT
#endif

#ifndef MFX_VERSION
#error MFX_VERSION not defined
#endif
const char* strAppName ="demo";

void PrintHelp(const msdk_char *strErrorMessage, ...)
{
    msdk_printf(MSDK_STRING("Encoding Sample Version %s\n\n"), GetMSDKSampleVersion().c_str());

    if (strErrorMessage)
    {
        va_list args;
        msdk_printf(MSDK_STRING("ERROR: "));
        va_start(args, strErrorMessage);
        msdk_vprintf(strErrorMessage, args);
        va_end(args);
        msdk_printf(MSDK_STRING("\n\n"));
    }

    msdk_printf(MSDK_STRING("Usage: %Ts <msdk-codecid> [<options>] -i InputYUVFile -o OutputEncodedFile -w width -h height\n"), strAppName);
    msdk_printf(MSDK_STRING("\n"));
    msdk_printf(MSDK_STRING("Supported codecs, <msdk-codecid>:\n"));
    msdk_printf(MSDK_STRING("   <codecid>=h264|mpeg2|vc1|mvc|jpeg - built-in Media SDK codecs\n"));
    msdk_printf(MSDK_STRING("   <codecid>=h265|vp9                - in-box Media SDK plugins (may require separate downloading and installation)\n"));
    msdk_printf(MSDK_STRING("   If codecid is jpeg, -q option is mandatory.)\n"));
    msdk_printf(MSDK_STRING("Options: \n"));
#ifdef MOD_ENC
    MOD_ENC_PRINT_HELP;
#endif
    msdk_printf(MSDK_STRING("   [-nv12|yuy2|ayuv|rgb4|p010|y210|y410|a2rgb10] - input color format (by default YUV420 is expected).\n"));
    msdk_printf(MSDK_STRING("   [-msb10] - 10-bit color format is expected to have data in Most Significant Bits of words.\n                 (LSB data placement is expected by default).\n                 This option also disables data shifting during file reading.\n"));
    msdk_printf(MSDK_STRING("   [-ec::p010] - force usage of P010 surfaces for encoder (conversion will be made if necessary). Use for 10 bit HEVC encoding\n"));
    msdk_printf(MSDK_STRING("   [-tff|bff] - input stream is interlaced, top|bottom fielf first, if not specified progressive is expected\n"));
    msdk_printf(MSDK_STRING("   [-bref] - arrange B frames in B pyramid reference structure\n"));
    msdk_printf(MSDK_STRING("   [-nobref] -  do not use B-pyramid (by default the decision is made by library)\n"));
    msdk_printf(MSDK_STRING("   [-idr_interval size] - idr interval, default 0 means every I is an IDR, 1 means every other I frame is an IDR etc\n"));
    msdk_printf(MSDK_STRING("   [-f frameRate] - video frame rate (frames per second)\n"));
    msdk_printf(MSDK_STRING("   [-n number] - number of frames to process\n"));
    msdk_printf(MSDK_STRING("   [-b bitRate] - encoded bit rate (Kbits per second), valid for H.264, H.265, MPEG2 and MVC encoders \n"));
    msdk_printf(MSDK_STRING("   [-u usage] - target usage, valid for H.265, H.264, H.265, MPEG2 and MVC encoders. Expected values:\n"));
    msdk_printf(MSDK_STRING("                veryslow(quality), slower, slow, medium(balanced), fast, faster, veryfast(speed)\n"));
    msdk_printf(MSDK_STRING("   [-q quality] - mandatory quality parameter for JPEG encoder. In range [1,100]. 100 is the best quality. \n"));
    msdk_printf(MSDK_STRING("   [-r distance] - Distance between I- or P- key frames (1 means no B-frames) \n"));
    msdk_printf(MSDK_STRING("   [-g size] - GOP size (default 256)\n"));
    msdk_printf(MSDK_STRING("   [-x numRefs]   - number of reference frames\n"));
    msdk_printf(MSDK_STRING("   [-num_active_P numRefs]   - number of maximum allowed references for P frames (for HEVC only)\n"));
    msdk_printf(MSDK_STRING("   [-num_active_BL0 numRefs] - number of maximum allowed references for B frames in L0 (for HEVC only)\n"));
    msdk_printf(MSDK_STRING("   [-num_active_BL1 numRefs] - number of maximum allowed references for B frames in L1 (for HEVC only)\n"));
    msdk_printf(MSDK_STRING("   [-la] - use the look ahead bitrate control algorithm (LA BRC) (by default constant bitrate control method is used)\n"));
    msdk_printf(MSDK_STRING("           for H.264, H.265 encoder. Supported only with -hw option on 4th Generation Intel Core processors. \n"));
    msdk_printf(MSDK_STRING("   [-lad depth] - depth parameter for the LA BRC, the number of frames to be analyzed before encoding. In range [10,100].\n"));
    msdk_printf(MSDK_STRING("            may be 1 in the case when -mss option is specified \n"));
    msdk_printf(MSDK_STRING("   [-dstw width] - destination picture width, invokes VPP resizing\n"));
    msdk_printf(MSDK_STRING("   [-dsth height] - destination picture height, invokes VPP resizing\n"));
    msdk_printf(MSDK_STRING("   [-hw] - use platform specific SDK implementation (default)\n"));
    msdk_printf(MSDK_STRING("   [-sw] - use software implementation, if not specified platform specific SDK implementation is used\n"));
    msdk_printf(MSDK_STRING("   [-p plugin] - encoder plugin. Supported values: hevce_sw, hevce_gacc, hevce_hw, vp8e_hw, vp9e_hw, hevce_fei_hw\n"));
    msdk_printf(MSDK_STRING("                              (optional for Media SDK in-box plugins, required for user-encoder ones)\n"));
    msdk_printf(MSDK_STRING("   [-path path] - path to plugin (valid only in pair with -p option)\n"));
    msdk_printf(MSDK_STRING("   [-async]                 - depth of asynchronous pipeline. default value is 4. must be between 1 and 20.\n"));
    msdk_printf(MSDK_STRING("   [-gpucopy::<on,off>] Enable or disable GPU copy mode\n"));
    msdk_printf(MSDK_STRING("   [-robust:soft]           - Recovery from GPU hang by inserting an IDR\n"));
    msdk_printf(MSDK_STRING("   [-vbr]                   - variable bitrate control\n"));
    msdk_printf(MSDK_STRING("   [-cbr]                   - constant bitrate control\n"));
    msdk_printf(MSDK_STRING("   [-qvbr quality]          - variable bitrate control algorithm with constant quality. Quality in range [1,51]. 1 is the best quality.\n"));
    msdk_printf(MSDK_STRING("   [-icq quality]           - Intelligent Constant Quality (ICQ) bitrate control method. In range [1,51]. 1 is the best quality.\n"));
    msdk_printf(MSDK_STRING("   [-cqp]                   - constant quantization parameter (CQP BRC) bitrate control method\n"));
    msdk_printf(MSDK_STRING("                              (by default constant bitrate control method is used), should be used along with -qpi, -qpp, -qpb.\n"));
    msdk_printf(MSDK_STRING("   [-qpi]                   - constant quantizer for I frames (if bitrace control method is CQP). In range [1,51]. 0 by default, i.e.no limitations on QP.\n"));
    msdk_printf(MSDK_STRING("   [-qpp]                   - constant quantizer for P frames (if bitrace control method is CQP). In range [1,51]. 0 by default, i.e.no limitations on QP.\n"));
    msdk_printf(MSDK_STRING("   [-qpb]                   - constant quantizer for B frames (if bitrace control method is CQP). In range [1,51]. 0 by default, i.e.no limitations on QP.\n"));
#if (MFX_VERSION >= 1027)
    msdk_printf(MSDK_STRING("   [-round_offset_in file]  - use this file to set per frame inter/intra rounding offset(for AVC only)\n"));
#endif
    msdk_printf(MSDK_STRING("   [-qsv-ff]       Enable QSV-FF mode\n"));
    msdk_printf(MSDK_STRING("   [-ir_type]               - Intra refresh type. 0 - no refresh, 1 - vertical refresh, 2 - horisontal refresh, 3 - slice refresh\n"));
    msdk_printf(MSDK_STRING("   [-ir_cycle_size]         - Number of pictures within refresh cycle starting from 2\n"));
    msdk_printf(MSDK_STRING("   [-ir_qp_delta]           - QP difference for inserted intra MBs. This is signed value in [-51, 51] range\n"));
    msdk_printf(MSDK_STRING("   [-ir_cycle_dist]         - Distance between the beginnings of the intra-refresh cycles in frames\n"));
    msdk_printf(MSDK_STRING("   [-gpb:<on,off>]          - Turn this option OFF to make HEVC encoder use regular P-frames instead of GPB\n"));
#if (MFX_VERSION >= 1026)
    msdk_printf(MSDK_STRING("  -TransformSkip:<on,off>- Turn this option ON to enable transformskip\n"));
#endif
    msdk_printf(MSDK_STRING("  -AvcTemporalLayers [array:Layer.Scale]    Configures the temporal layers hierarchy\n"));
    msdk_printf(MSDK_STRING("  -BaseLayerPID <pid>                       Sets priority ID for the base layer\n"));
    msdk_printf(MSDK_STRING("  -SPSId <pid>                              Sets sequence parameter set ID\n"));
    msdk_printf(MSDK_STRING("  -PPSId <pid>                              Sets picture parameter set ID\n"));
    msdk_printf(MSDK_STRING("  -PicTimingSEI:<on,off>                    Enables or disables picture timing SEI\n"));
    msdk_printf(MSDK_STRING("  -NalHrdConformance:<on,off>               Enables or disables picture HRD conformance\n"));
    msdk_printf(MSDK_STRING("  -VuiNalHrdParameters:<on,off>             Enables or disables NAL HRD parameters in VUI header\n"));
    msdk_printf(MSDK_STRING("   [-ppyr:<on,off>]         - Turn this option ON to enable P-pyramid (by default the decision is made by library)\n"));
    msdk_printf(MSDK_STRING("   [-num_slice]             - number of slices in each video frame. 0 by default.\n"));
    msdk_printf(MSDK_STRING("                              If num_slice equals zero, the encoder may choose any slice partitioning allowed by the codec standard.\n"));
    msdk_printf(MSDK_STRING("   [-mss]                   - maximum slice size in bytes. Supported only with -hw and h264 codec. This option is not compatible with -num_slice option.\n"));
    msdk_printf(MSDK_STRING("   [-mfs]                   - maximum frame size in bytes. Supported only with h264 and hevc codec for VBR mode.\n"));
    msdk_printf(MSDK_STRING("   [-re]                    - enable region encode mode. Works only with h265 encoder\n"));
    msdk_printf(MSDK_STRING("   [-trows rows]            - Number of rows for tiled encoding\n"));
    msdk_printf(MSDK_STRING("   [-tcols cols]            - Number of columns for tiled encoding\n"));
    msdk_printf(MSDK_STRING("   [-CodecProfile]          - specifies codec profile\n"));
    msdk_printf(MSDK_STRING("   [-CodecLevel]            - specifies codec level\n"));
    msdk_printf(MSDK_STRING("   [-GopOptFlag:closed]     - closed gop\n"));
    msdk_printf(MSDK_STRING("   [-GopOptFlag:strict]     - strict gop\n"));
    msdk_printf(MSDK_STRING("   [-AdaptiveI:<on,off>]    - Turn Adaptive I frames on/off\n"));
    msdk_printf(MSDK_STRING("   [-AdaptiveB:<on,off>]    - Turn Adaptive B frames on/off\n"));
    msdk_printf(MSDK_STRING("   [-InitialDelayInKB]      - the decoder starts decoding after the buffer reaches the initial size InitialDelayInKB, \
                            which is equivalent to reaching an initial delay of InitialDelayInKB*8000/TargetKbps ms\n"));
    msdk_printf(MSDK_STRING("   [-BufferSizeInKB ]       - represents the maximum possible size of any compressed frames\n"));
    msdk_printf(MSDK_STRING("   [-MaxKbps ]              - for variable bitrate control, specifies the maximum bitrate at which \
                            the encoded data enters the Video Buffering Verifier buffer\n"));
    msdk_printf(MSDK_STRING("   [-ws]                    - specifies sliding window size in frames\n"));
    msdk_printf(MSDK_STRING("   [-wb]                    - specifies the maximum bitrate averaged over a sliding window in Kbps\n"));
    msdk_printf(MSDK_STRING("   [-amfs:<on,off>]         - adaptive max frame size. If set on, P or B frame size can exceed MaxFrameSize when the scene change is detected.\
                            It can benefit the video quality \n"));
#if (MFX_VERSION >= 1023)
    msdk_printf(MSDK_STRING("   [-LowDelayBRC]           - strictly obey average frame size set by MaxKbps\n"));
#endif
    msdk_printf(MSDK_STRING("   [-signal:tm ]            - represents transfer matrix coefficients for mfxExtVideoSignalInfo. 0 - unknown, 1 - BT709, 2 - BT601\n"));
    msdk_printf(MSDK_STRING("   [-WeightedPred:default|implicit ]   - enables weighted prediction mode\n"));
    msdk_printf(MSDK_STRING("   [-WeightedBiPred:default|implicit ] - enables weighted bi-prediction mode\n"));
    msdk_printf(MSDK_STRING("   [-timeout]               - encoding in cycle not less than specific time in seconds\n"));
    msdk_printf(MSDK_STRING("   [-membuf]                - size of memory buffer in frames\n"));
    msdk_printf(MSDK_STRING("   [-uncut]                 - do not cut output file in looped mode (in case of -timeout option)\n"));
    msdk_printf(MSDK_STRING("   [-dump fileName]         - dump MSDK components configuration to the file in text form\n"));
    msdk_printf(MSDK_STRING("   [-usei]                  - insert user data unregistered SEI. eg: 7fc92488825d11e7bb31be2e44b06b34:0:MSDK (uuid:type<0-preifx/1-suffix>:message)\n"));
    msdk_printf(MSDK_STRING("                              the suffix SEI for HEVCe can be inserted when CQP used or HRD disabled\n"));
#if (MFX_VERSION >= MFX_VERSION_NEXT)
    msdk_printf(MSDK_STRING("   [-dblk_alpha]            - alpha deblocking parameter. In range[-12,12]. 0 by default.\n"));
    msdk_printf(MSDK_STRING("   [-dblk_beta]             - beta deblocking parameter. In range[-12,12]. 0 by default.\n"));
#endif
#if (MFX_VERSION >= 1024)
    msdk_printf(MSDK_STRING("   [-extbrc:<on,off,implicit>] - External BRC for AVC and HEVC encoders\n"));
#endif
#if (MFX_VERSION >= 1026)
    msdk_printf(MSDK_STRING("   [-ExtBrcAdaptiveLTR:<on,off>] - Set AdaptiveLTR for implicit extbrc"));
#endif
    msdk_printf(MSDK_STRING("Example: %s h265 -i InputYUVFile -o OutputEncodedFile -w width -h height -hw -p 2fca99749fdb49aeb121a5b63ef568f7\n"), strAppName);
    msdk_printf(MSDK_STRING("   [-preset <default,dss,conference,gaming>] - Use particular preset for encoding parameters\n"));
    msdk_printf(MSDK_STRING("   [-pp] - Print preset parameters\n"));    msdk_printf(MSDK_STRING("\nExample: %s h265 -i InputYUVFile -o OutputEncodedFile -w width -h height -hw -p 2fca99749fdb49aeb121a5b63ef568f7\n"), strAppName);
#if D3D_SURFACES_SUPPORT
    msdk_printf(MSDK_STRING("   [-d3d] - work with d3d surfaces\n"));
    msdk_printf(MSDK_STRING("   [-d3d11] - work with d3d11 surfaces\n"));
    msdk_printf(MSDK_STRING("   [-single_texture_d3d11 ] - single texture mode for d3d11 allocator \n"));
    msdk_printf(MSDK_STRING("Example: %s h264|h265|mpeg2|jpeg -i InputYUVFile -o OutputEncodedFile -w width -h height -d3d -hw \n"), strAppName);
    msdk_printf(MSDK_STRING("Example for MVC: %s mvc -i InputYUVFile_1 -i InputYUVFile_2 -o OutputEncodedFile -w width -h height \n"), strAppName);
#endif
#ifdef LIBVA_SUPPORT
    msdk_printf(MSDK_STRING("   [-vaapi] - work with vaapi surfaces\n"));
    msdk_printf(MSDK_STRING("Example: %s h264|mpeg2|mvc -i InputYUVFile -o OutputEncodedFile -w width -h height -angle 180 -g 300 -r 1 \n"), strAppName);
#endif
#if defined (ENABLE_V4L2_SUPPORT)
    msdk_printf(MSDK_STRING("   [-d]                            - Device video node (eg: /dev/video0)\n"));
    msdk_printf(MSDK_STRING("   [-p]                            - Mipi Port number (eg: Port 0)\n"));
    msdk_printf(MSDK_STRING("   [-m]                            - Mipi Mode Configuration [PREVIEW/CONTINUOUS/STILL/VIDEO]\n"));
    msdk_printf(MSDK_STRING("   [-uyvy]                        - Input Raw format types V4L2 Encode\n"));
    msdk_printf(MSDK_STRING("   [-YUY2]                        - Input Raw format types V4L2 Encode\n"));
    msdk_printf(MSDK_STRING("   [-i::v4l2]                        - To enable v4l2 option\n"));
    msdk_printf(MSDK_STRING("Example: %s h264|mpeg2|mvc -i::v4l2 -o OutputEncodedFile -w width -h height -d /dev/video0 -uyvy -m preview -p 0\n"), strAppName);
#endif
    msdk_printf(MSDK_STRING("   [-viewoutput] - instruct the MVC encoder to output each view in separate bitstream buffer. Depending on the number of -o options behaves as follows:\n"));
    msdk_printf(MSDK_STRING("                   1: two views are encoded in single file\n"));
    msdk_printf(MSDK_STRING("                   2: two views are encoded in separate files\n"));
    msdk_printf(MSDK_STRING("                   3: behaves like 2 -o opitons was used and then one -o\n\n"));
    msdk_printf(MSDK_STRING("Example: %s mvc -i InputYUVFile_1 -i InputYUVFile_2 -o OutputEncodedFile_1 -o OutputEncodedFile_2 -viewoutput -w width -h height \n"), strAppName);
    // user module options
    msdk_printf(MSDK_STRING("User module options: \n"));
    msdk_printf(MSDK_STRING("   [-angle 180] - enables 180 degrees picture rotation before encoding, CPU implementation by default. Rotation requires NV12 input. Options -tff|bff, -dstw, -dsth, -d3d are not effective together with this one, -nv12 is required.\n"));
    msdk_printf(MSDK_STRING("   [-opencl] - rotation implementation through OPENCL\n"));
    msdk_printf(MSDK_STRING("Example: %s h264|h265|mpeg2|mvc|jpeg -i InputYUVFile -o OutputEncodedFile -w width -h height -angle 180 -opencl \n"), strAppName);

    msdk_printf(MSDK_STRING("\n"));
}

mfxStatus ParseInputString(sInputParams* pParams)
{

#if 1
	msdk_printf(MSDK_STRING("[DEBUG]ParseInputString ----------------------------------------------IN\n"));
	Config config("config.txt");
	std::string setting;
	printf("InputFile : %s\n", config.Read<std::string>("InputFile").c_str());
	printf("OutputFile : %s\n", config.Read<std::string>("OutputFile").c_str());

	printf("InputWidth : %d\n", config.Read<int>("InputWidth"));
	printf("InputHeight : %d\n", config.Read<int>("InputHeight"));
	printf("EncoderMode : %d\n", config.Read<int>("EncoderMode"));
	printf("CodecType : %s\n", config.Read<std::string>("CodecType").c_str());
#endif

    MSDK_CHECK_POINTER(pParams, MFX_ERR_NULL_PTR);
    msdk_opt_read(MSDK_CPU_ROTATE_PLUGIN, pParams->strPluginDLLPath);

    // default implementation
    pParams->bUseHWLib = true;
    pParams->isV4L2InputEnabled = false;
    pParams->nNumFrames = 0;
    pParams->FileInputFourCC = MFX_FOURCC_I420;
    pParams->EncodeFourCC = MFX_FOURCC_NV12;
    pParams->nPRefType = MFX_P_REF_DEFAULT;
#if (MFX_VERSION >= 1027)
    pParams->RoundingOffsetFile = NULL;
#endif

	wchar_t ws[256];
	swprintf(ws, 256, L"%hs", config.Read<std::string>("CodecType").c_str());
	mfxStatus sts = StrFormatToCodecFormatFourCC(ws, pParams->CodecId);

	if (sts != MFX_ERR_NONE)
	{
		msdk_printf(MSDK_STRING("[DEBUG]Unknown codec %s\n"), config.Read<std::string>("CodecType").c_str());
		return MFX_ERR_UNSUPPORTED;
	}
	if (!IsEncodeCodecSupported(pParams->CodecId))
	{
		msdk_printf(MSDK_STRING("[DEBUG]Unsupported codec %s\n"), config.Read<std::string>("CodecType").c_str());
		return MFX_ERR_UNSUPPORTED;
	}
	if (pParams->CodecId == CODEC_MVC)
	{
		pParams->CodecId = MFX_CODEC_AVC;
		pParams->MVC_flags |= MVC_ENABLED;
	}

	if (0 == config.Read<int>("EncoderMode"))
	{
		pParams->bUseHWLib = true;
	}
	else if (1 == config.Read<int>("EncoderMode"))
	{
		pParams->bUseHWLib = false;
	}

	if (0 == config.Read<int>("RateControl"))
	{
		pParams->nRateControlMethod = MFX_RATECONTROL_VBR;
	}
	else if (1 == config.Read<int>("RateControl"))
	{
		pParams->nRateControlMethod = MFX_RATECONTROL_CBR;
	}

	if (-1 != config.Read<int>("TargetBitrate"))
	{
		pParams->nBitRate = config.Read<int>("TargetBitrate");
	}else {
		msdk_printf(MSDK_STRING("[DEBUG]Bitrate(%d) is invalid\n"), config.Read<int>("TargetBitrate"));
		return MFX_ERR_UNSUPPORTED;
	}

	if (-1 != config.Read<int>("FrameRate"))
	{
		pParams->dFrameRate = config.Read<int>("FrameRate");
	}
	else {
		msdk_printf(MSDK_STRING("[DEBUG]Framerate(%d) is invalid\n"), config.Read<int>("TargetBitrate"));
		return MFX_ERR_UNSUPPORTED;
	}
	
	if (!config.Read<std::string>("InputFile").empty())
	{
		memset(ws,sizeof(wchar_t),256);
		swprintf(ws, 256, L"%hs", config.Read<std::string>("InputFile").c_str());
		pParams->InputFiles.push_back(ws);
	}
	
	if (!config.Read<std::string>("OutputFile").empty())
	{
		memset(ws, sizeof(wchar_t), 256);
		swprintf(ws, 256, L"%hs", config.Read<std::string>("OutputFile").c_str());
		//printf("[debug][CSmplBitstreamWriter::Init]--------------------ws[wchar_t]=%ls,ws[char]=%s\r\n", ws, ws);
		pParams->dstFileBuff.push_back(_T(".\\enc.h265"));
		//pParams->dstFileBuff.push_back(_T("D:\\work\\test\\intel_qsv\\intel_qsv\\_build\\x64\\Debug\\sc_desktop_1920x1080_60_8bit_420.h265"));
		//pParams->dstFileBuff.push_back(ws);
		//printf("[debug][CSmplBitstreamWriter::Init]--------------------size=%d **** dstFileBuff[wchar_t]=%ls\r\n", pParams->dstFileBuff.size(), pParams->dstFileBuff[0]);
	}
	
    // check if all mandatory parameters were set
    if (!pParams->InputFiles.size())
    {
		msdk_printf(MSDK_STRING("Source file name not found"));
        return MFX_ERR_UNSUPPORTED;
    };

	pParams->nWidth = config.Read<int>("InputWidth");
	pParams->nHeight = config.Read<int>("InputHeight");
    if (0 == pParams->nWidth || 0 == pParams->nHeight)
    {
		msdk_printf(MSDK_STRING("InputWidth,InputHeight must be specified"));
        return MFX_ERR_UNSUPPORTED;
    }

    if (MFX_CODEC_JPEG != pParams->CodecId && MFX_CODEC_HEVC != pParams->CodecId &&
        pParams->FileInputFourCC == MFX_FOURCC_YUY2 &&
        !pParams->isV4L2InputEnabled)
    {
		msdk_printf(MSDK_STRING("-yuy2 option is supported only for JPEG or HEVC encoder"));
        return MFX_ERR_UNSUPPORTED;
    }

    if (MFX_CODEC_HEVC != pParams->CodecId && MFX_CODEC_VP9 != pParams->CodecId && (pParams->EncodeFourCC == MFX_FOURCC_P010) )
    {
		msdk_printf(MSDK_STRING("P010 surfaces are supported only for HEVC and VP9 encoder"));
        return MFX_ERR_UNSUPPORTED;
    }

    if ((pParams->nTargetUsage || pParams->nBitRate) && (MFX_CODEC_JPEG == pParams->CodecId))
    {
		msdk_printf(MSDK_STRING("-u and -b options are supported only for H.264, MPEG2 and MVC encoders. For JPEG encoder use -q"));
        return MFX_ERR_UNSUPPORTED;
    }

    if (!pParams->nQuality && (MFX_CODEC_JPEG == pParams->CodecId))
    {
		msdk_printf(MSDK_STRING("-q must be specified for JPEG encoder"));
        return MFX_ERR_UNSUPPORTED;
    }

    if (MFX_TRANSFERMATRIX_UNKNOWN != pParams->TransferMatrix &&
        MFX_TRANSFERMATRIX_BT601 != pParams->TransferMatrix &&
        MFX_TRANSFERMATRIX_BT709 != pParams->TransferMatrix)
    {
		msdk_printf(MSDK_STRING("Invalid transfer matrix type"));
        return MFX_ERR_UNSUPPORTED;
    }

    // set default values for optional parameters that were not set or were set incorrectly
    mfxU32 nviews = (mfxU32)pParams->InputFiles.size();
    if ((nviews <= 1) || (nviews > 2))
    {
        if (!(MVC_ENABLED & pParams->MVC_flags))
        {
            pParams->numViews = 1;
        }
        else
        {
			msdk_printf(MSDK_STRING("Only 2 views are supported right now in this sample."));
            return MFX_ERR_UNSUPPORTED;
        }
    }
    else
    {
        pParams->numViews = nviews;
    }

    if (pParams->dFrameRate <= 0)
    {
        pParams->dFrameRate = 30;
    }

    // if no destination picture width or height wasn't specified set it to the source picture size
    if (pParams->nDstWidth == 0)
    {
        pParams->nDstWidth = pParams->nWidth;
    }

    if (pParams->nDstHeight == 0)
    {
        pParams->nDstHeight = pParams->nHeight;
    }

    if (!pParams->nPicStruct)
    {
        pParams->nPicStruct = MFX_PICSTRUCT_PROGRESSIVE;
    }

    if ((pParams->nRateControlMethod == MFX_RATECONTROL_LA) && (!pParams->bUseHWLib))
    {
		msdk_printf(MSDK_STRING("Look ahead BRC is supported only with -hw option!"));
        return MFX_ERR_UNSUPPORTED;
    }

    if ((pParams->nMaxSliceSize) && (!pParams->bUseHWLib))
    {
        PrintHelp( MSDK_STRING("MaxSliceSize option is supported only with -hw option!"));
        return MFX_ERR_UNSUPPORTED;
    }

    if ((pParams->nMaxSliceSize) && (pParams->nNumSlice))
    {
        PrintHelp(MSDK_STRING("-mss and -num_slice options are not compatible!"));
        return MFX_ERR_UNSUPPORTED;
    }

    if ((pParams->nMaxSliceSize) && (pParams->CodecId != MFX_CODEC_AVC))
    {
        PrintHelp( MSDK_STRING("MaxSliceSize option is supported only with H.264 encoder!"));
        return MFX_ERR_UNSUPPORTED;
    }

    if (pParams->nLADepth && (pParams->nLADepth < 10))
    {
        if ((pParams->nLADepth != 1) || (!pParams->nMaxSliceSize))
        {
            PrintHelp( MSDK_STRING("Unsupported value of -lad parameter, must be >= 10, or 1 in case of -mss option!"));
            return MFX_ERR_UNSUPPORTED;
        }
    }

    if (pParams->nNumRefActiveP || pParams->nNumRefActiveBL0 || pParams->nNumRefActiveBL1)
    {
        if (pParams->CodecId != MFX_CODEC_HEVC)
        {
            PrintHelp(MSDK_STRING("NumRefActiveP/BL0/BL1 are supported only with HEVC encoder"));
            return MFX_ERR_UNSUPPORTED;
        }
    }

    // not all options are supported if rotate plugin is enabled
    if (pParams->nRotationAngle == 180 && (
        MFX_PICSTRUCT_PROGRESSIVE != pParams->nPicStruct ||
        pParams->nDstWidth != pParams->nWidth ||
        pParams->nDstHeight != pParams->nHeight ||
        MVC_ENABLED & pParams->MVC_flags ||
        pParams->nRateControlMethod == MFX_RATECONTROL_LA))
    {
        PrintHelp(MSDK_STRING("Some of the command line options are not supported with rotation plugin!"));
        return MFX_ERR_UNSUPPORTED;
    }

    // Ignoring user-defined Async Depth for LA
    if (pParams->nMaxSliceSize)
    {
        pParams->nAsyncDepth = 1;
    }

    if(pParams->UseRegionEncode)
    {
        if(pParams->CodecId != MFX_CODEC_HEVC)
        {
            msdk_printf(MSDK_STRING("Region encode option is compatible with h265(HEVC) encoder only.\nRegion encoding is disabled\n"));
            pParams->UseRegionEncode=false;
        }
        if (pParams->nWidth  != pParams->nDstWidth ||
            pParams->nHeight != pParams->nDstHeight ||
            pParams->nRotationAngle!=0)
        {
            msdk_printf(MSDK_STRING("Region encode option is not compatible with VPP processing and rotation plugin.\nRegion encoding is disabled\n"));
            pParams->UseRegionEncode=false;
        }
    }

    if (pParams->dstFileBuff.size() == 0)
    {
        msdk_printf(MSDK_STRING("File output is disabled as -o option isn't specified\n"));
    }
	printf("xx[debug][CSmplBitstreamWriter::Init]--------------------size=%d **** dstFileBuff[wchar_t]=%ls\r\n", pParams->dstFileBuff.size(), pParams->dstFileBuff[0]);
	msdk_printf(MSDK_STRING("[DEBUG]ParseInputString ----------------------------------------------OUT\n"));
    return MFX_ERR_NONE;
}


void ModifyParamsUsingPresets(sInputParams& params)
{
    COutputPresetParameters presetParams = CPresetManager::Inst.GetPreset(params.PresetMode, params.CodecId,params.dFrameRate, params.nWidth, params.nHeight, params.bUseHWLib);

    if (presetParams.RateControlMethod == MFX_RATECONTROL_LA_EXT)
    {
        // Currently sample_encode does not support external LA, so we will use ExtBRC instead
        presetParams.RateControlMethod = MFX_RATECONTROL_VBR;
        presetParams.LookAheadDepth = 0;
        presetParams.ExtBRCUsage = EXTBRC_ON;
    }

    if (params.shouldPrintPresets)
    {
        msdk_printf(MSDK_STRING("Preset-controlled parameters (%s):\n"), presetParams.PresetName.c_str());
    }
    MODIFY_AND_PRINT_PARAM(params.nAdaptiveMaxFrameSize, AdaptiveMaxFrameSize,params.shouldPrintPresets);
    MODIFY_AND_PRINT_PARAM(params.nAsyncDepth, AsyncDepth, params.shouldPrintPresets);
    MODIFY_AND_PRINT_PARAM(params.nBRefType, BRefType, params.shouldPrintPresets);
//    MODIFY_AND_PRINT_PARAM(params., EnableBPyramid);
//    MODIFY_AND_PRINT_PARAM(params., EnablePPyramid);
    MODIFY_AND_PRINT_PARAM(params.nGopPicSize, GopPicSize, params.shouldPrintPresets);
    MODIFY_AND_PRINT_PARAM(params.nGopRefDist, GopRefDist, params.shouldPrintPresets);
    MODIFY_AND_PRINT_PARAM(params.IntRefCycleDist, IntRefCycleDist, params.shouldPrintPresets);
    MODIFY_AND_PRINT_PARAM(params.IntRefCycleSize, IntRefCycleSize, params.shouldPrintPresets);
    MODIFY_AND_PRINT_PARAM(params.IntRefQPDelta, IntRefQPDelta, params.shouldPrintPresets);
    MODIFY_AND_PRINT_PARAM(params.IntRefType, IntRefType, params.shouldPrintPresets);
    MODIFY_AND_PRINT_PARAM(params.nLADepth, LookAheadDepth, params.shouldPrintPresets);
    MODIFY_AND_PRINT_PARAM(params.LowDelayBRC, LowDelayBRC, params.shouldPrintPresets);
    MODIFY_AND_PRINT_PARAM(params.nMaxFrameSize, MaxFrameSize, params.shouldPrintPresets);
    if (!params.nRateControlMethod)
    {
        // Use preset ExtBRC parameter only if rate control mode was not set manually
        MODIFY_AND_PRINT_PARAM_EXT(params.nExtBRC, ExtBRCUsage, (ExtBRCType)presetParams.ExtBRCUsage, params.shouldPrintPresets);
    }
    MODIFY_AND_PRINT_PARAM(params.nRateControlMethod, RateControlMethod, params.shouldPrintPresets);

    if (params.nRateControlMethod != MFX_RATECONTROL_CQP)
    {
        MODIFY_AND_PRINT_PARAM(params.nBitRate, TargetKbps, params.shouldPrintPresets);
        MODIFY_AND_PRINT_PARAM(params.MaxKbps, MaxKbps, params.shouldPrintPresets);
        presetParams.BufferSizeInKB = params.nBitRate; // Update bitrate to reflect manually set bitrate. BufferSize should be enough for 1 second of video
        MODIFY_AND_PRINT_PARAM(params.BufferSizeInKB, BufferSizeInKB, params.shouldPrintPresets);
    }

    MODIFY_AND_PRINT_PARAM(params.nTargetUsage, TargetUsage, params.shouldPrintPresets);
    MODIFY_AND_PRINT_PARAM(params.WeightedBiPred, WeightedBiPred, params.shouldPrintPresets);
    MODIFY_AND_PRINT_PARAM(params.WeightedPred, WeightedPred, params.shouldPrintPresets);
    if (params.shouldPrintPresets)
    {
        msdk_printf(MSDK_STRING("\n"));
    }
}

CEncodingPipeline* CreatePipeline(const sInputParams& params)
{
#ifdef MOD_ENC
    MOD_ENC_CREATE_PIPELINE;
#endif
    if(params.UseRegionEncode)
    {
        return new CRegionEncodingPipeline;
    }
    else if(params.nRotationAngle)
    {
        return new CUserPipeline;
    }
    else
    {
        return new CEncodingPipeline;
    }
}


#if defined(_WIN32) || defined(_WIN64)
int _tmain(int argc, msdk_char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    sInputParams Params = {};   // input parameters from command line
    std::unique_ptr<CEncodingPipeline>  pPipeline;

    mfxStatus sts = MFX_ERR_NONE; // return value check

    // Parsing Input stream workign with presets
    sts = ParseInputString(&Params);
	//printf("\r\n----debug][main]--------------------size=%d ^^^ dstFileBuff[wchar_t]=%ls\r\n", Params.dstFileBuff.size(), Params.dstFileBuff[0]);
    ModifyParamsUsingPresets(Params);

    MSDK_CHECK_PARSE_RESULT(sts, MFX_ERR_NONE, 1);

    // Choosing which pipeline to use
    pPipeline.reset(CreatePipeline(Params));
    MSDK_CHECK_POINTER(pPipeline.get(), MFX_ERR_MEMORY_ALLOC);

    if (MVC_ENABLED & Params.MVC_flags)
    {
        pPipeline->SetNumView(Params.numViews);
    }

    sts = pPipeline->Init(&Params);
    MSDK_CHECK_STATUS(sts, "pPipeline->Init failed");

    pPipeline->PrintInfo();
    msdk_printf(MSDK_STRING("[DEBUG]--->Processing started\n"));

    for (;;)
    {
        sts = pPipeline->Run();

        if (MFX_ERR_DEVICE_LOST == sts || MFX_ERR_DEVICE_FAILED == sts)
        {
            msdk_printf(MSDK_STRING("\nERROR: Hardware device was lost or returned an unexpected error. Recovering...\n"));
            sts = pPipeline->ResetDevice();
            MSDK_CHECK_STATUS(sts, "pPipeline->ResetDevice failed");

            sts = pPipeline->ResetMFXComponents(&Params);
            MSDK_CHECK_STATUS(sts, "pPipeline->ResetMFXComponents failed");
            continue;
        }
        else
        {
            MSDK_CHECK_STATUS(sts, "pPipeline->Run failed");
            break;
        }
    }

    pPipeline->Close();

    msdk_printf(MSDK_STRING("\n[DEBUG]--->Processing finished\n"));

    return 0;
}
