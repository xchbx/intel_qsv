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

#pragma once

#if !defined(_WIN32) && !defined(_WIN64)

#include "logger.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include <va/va.h>

#define DCL_USE_DEPRECATED_OPENCL_1_1_APIS
#include <CL/cl.h>
#include <CL/opencl.h>
#include <CL/cl_va_api_media_sharing_intel.h>

#include "opencl_filter.h"

class OpenCLFilterVA : public OpenCLFilterBase
{
public:
    OpenCLFilterVA();
    virtual ~OpenCLFilterVA();
    virtual cl_int OCLInit(mfxHDL device);

protected: // functions
    virtual cl_int InitDevice();
    virtual cl_int InitSurfaceSharingExtension();

    virtual cl_mem CreateSharedSurface(mfxMemId mid, int nView, bool bIsReadOnly);
    virtual bool EnqueueAcquireSurfaces(cl_mem* surfaces, int nSurfaces);
    virtual bool EnqueueReleaseSurfaces(cl_mem* surfaces, int nSurfaces);

protected: // variables
    VADisplay m_vaDisplay;
    VASurfaceID m_SharedSurfaces[c_shared_surfaces_num];
};

#endif // #if !defined(_WIN32) && !defined(_WIN64)
