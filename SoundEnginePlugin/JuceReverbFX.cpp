/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the
"Apache License"); you may not use this file except in compliance with the
Apache License. You may obtain a copy of the Apache License at
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2022 Audiokinetic Inc.
*******************************************************************************/

#include "JuceReverbFX.h"
#include "../JuceReverbConfig.h"

#include <AK/AkWwiseSDKVersion.h>

AK::IAkPlugin *CreateJuceReverbFX(AK::IAkPluginMemAlloc *in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, JuceReverbFX());
}

AK::IAkPluginParam *CreateJuceReverbFXParams(AK::IAkPluginMemAlloc *in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, JuceReverbFXParams());
}

AK_IMPLEMENT_PLUGIN_FACTORY(JuceReverbFX, AkPluginTypeEffect, JuceReverbConfig::CompanyID, JuceReverbConfig::PluginID)

JuceReverbFX::JuceReverbFX()
    : m_pParams(nullptr), m_pAllocator(nullptr), m_pContext(nullptr)
{
}

JuceReverbFX::~JuceReverbFX()
{
}

AKRESULT JuceReverbFX::Init(AK::IAkPluginMemAlloc *in_pAllocator, AK::IAkEffectPluginContext *in_pContext, AK::IAkPluginParam *in_pParams, AkAudioFormat &in_rFormat)
{
    m_pParams = (JuceReverbFXParams *)in_pParams;
    m_pAllocator = in_pAllocator;
    m_pContext = in_pContext;

    return AK_Success;
}

AKRESULT JuceReverbFX::Term(AK::IAkPluginMemAlloc *in_pAllocator)
{
    AK_PLUGIN_DELETE(in_pAllocator, this);
    return AK_Success;
}

AKRESULT JuceReverbFX::Reset()
{
    return AK_Success;
}

AKRESULT JuceReverbFX::GetPluginInfo(AkPluginInfo &out_rPluginInfo)
{
    out_rPluginInfo.eType = AkPluginTypeEffect;
    out_rPluginInfo.bIsInPlace = true;
    out_rPluginInfo.bCanProcessObjects = false;
    out_rPluginInfo.uBuildVersion = AK_WWISESDK_VERSION_COMBINED;
    return AK_Success;
}

void JuceReverbFX::Execute(AkAudioBuffer *io_pBuffer)
{
    io_pBuffer->ZeroPadToMaxFrames();
    const AkUInt32 uNumChannels = io_pBuffer->NumChannels();
    const auto uMaxFrames = io_pBuffer->MaxFrames();

    reverbParams.roomSize = m_pParams->RTPC.fRoomSize;
    reverbParams.damping = m_pParams->RTPC.fDamping;
    reverbParams.width = m_pParams->RTPC.fWidth;
    reverbParams.wetLevel = m_pParams->RTPC.fMix;
    reverbParams.dryLevel = 1.0f - m_pParams->RTPC.fMix;
    reverbProcesser.setParameters(reverbParams);

    for (AkUInt32 i = 0; i < uNumChannels; ++i)
    {
        AkSampleType *data = io_pBuffer->GetChannel(i);
        juce::dsp::AudioBlock<AkSampleType> block(&data, 1, uMaxFrames);
        juce::dsp::ProcessContextReplacing<AkSampleType> context(block);
        reverbProcesser.process(context);
    }
}

AKRESULT JuceReverbFX::TimeSkip(AkUInt32 in_uFrames)
{
    return AK_DataReady;
}
