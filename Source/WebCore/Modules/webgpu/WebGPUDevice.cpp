/*
 * Copyright (C) 2018 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "WebGPUDevice.h"

#if ENABLE(WEBGPU)

#include "GPUPipelineStageDescriptor.h"
#include "GPURenderPipelineDescriptor.h"
#include "GPUShaderModuleDescriptor.h"
#include "Logging.h"
#include "WebGPUPipelineStageDescriptor.h"
#include "WebGPURenderPipeline.h"
#include "WebGPURenderPipelineDescriptor.h"
#include "WebGPUShaderModule.h"
#include "WebGPUShaderModuleDescriptor.h"

namespace WebCore {

RefPtr<WebGPUDevice> WebGPUDevice::create(Ref<WebGPUAdapter>&& adapter)
{
    auto device = GPUDevice::create(); // FIXME: Take adapter into account when creating m_device.
    if (!device)
        return nullptr;

    return adoptRef(new WebGPUDevice(WTFMove(adapter), WTFMove(device)));
}

WebGPUDevice::WebGPUDevice(Ref<WebGPUAdapter>&& adapter, RefPtr<GPUDevice>&& device)
    : m_adapter(WTFMove(adapter))
    , m_device(device)
{
    UNUSED_PARAM(m_adapter);
}

RefPtr<WebGPUShaderModule> WebGPUDevice::createShaderModule(WebGPUShaderModuleDescriptor&& descriptor) const
{
    return WebGPUShaderModule::create(m_device->createShaderModule(GPUShaderModuleDescriptor { descriptor.code }));
}

RefPtr<WebGPURenderPipeline> WebGPUDevice::createRenderPipeline(WebGPURenderPipelineDescriptor&& descriptor) const
{
    Vector<GPUPipelineStageDescriptor> stages;
    for (const auto& stageDescriptor : descriptor.stages) {
        if (!stageDescriptor.module) {
            LOG(WebGPU, "WebGPUDevice::createRenderPipeline(): WebGPUShaderModule not found!");
            return nullptr;
        }
        stages.append({ stageDescriptor.module->module(), stageDescriptor.stage, stageDescriptor.entryPoint });
    }

    return WebGPURenderPipeline::create(m_device->createRenderPipeline(GPURenderPipelineDescriptor { WTFMove(stages), static_cast<int>(descriptor.primitiveTopology) }));
}


} // namespace WebCore

#endif // ENABLE(WEBGPU)
