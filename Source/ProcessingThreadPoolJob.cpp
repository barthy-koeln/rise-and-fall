//
// Created by Barthélémy Bonhomme on 17.02.18.
//

#define AVOID_REALLOCATING false

#include "ProcessingThreadPoolJob.h"

ProcessingThreadPoolJob::ProcessingThreadPoolJob(ThreadType type, AudioSampleBuffer &bufferIn, GUIParams &guiParams,
                                                 double sampleRate)
        : ThreadPoolJob("Processing Thread Pool Job") {
    this->type = type;
    this->bufferIn = bufferIn;
    this->guiParams = guiParams;
    this->sampleRate = sampleRate;

    soundTouch.setChannels(1); // always iterate over single channels
    soundTouch.setSampleRate(static_cast<uint>(sampleRate));
}

void ProcessingThreadPoolJob::applyTimeWarp(AudioSampleBuffer &buffer, int factor) {
    float realFactor = factor;
    if (realFactor < 0) {
        realFactor = 1 / abs(realFactor);
    }

    AudioSampleBuffer copy;
    copy.makeCopyOf(buffer);

    soundTouch.setTempo(realFactor);
    soundTouch.setPitch(1.0);

    double ratio = soundTouch.getInputOutputSampleRatio();
    buffer.setSize(buffer.getNumChannels(), static_cast<int>(ceil(buffer.getNumSamples() * ratio)), false, true,
                   AVOID_REALLOCATING);

    for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
        soundTouch.putSamples(copy.getReadPointer(channel), static_cast<uint>(copy.getNumSamples()));
        soundTouch.receiveSamples(buffer.getWritePointer(channel), static_cast<uint>(buffer.getNumSamples()));
        soundTouch.clear();
    }
}

void ProcessingThreadPoolJob::applyDelay(AudioSampleBuffer &target, AudioSampleBuffer &base, float dampen,
                                         int delayTimeInSamples, int iteration) {
    base.applyGain(dampen);
    if (base.getMagnitude(0, base.getNumSamples()) > 0.05) {
        int currentDelayPosition = delayTimeInSamples * iteration;
        int length = target.getNumSamples() + base.getNumSamples() + delayTimeInSamples;
        target.setSize(target.getNumChannels(), length, true, true, AVOID_REALLOCATING);

        for (int channel = 0; channel < target.getNumChannels(); channel++) {
            for (int i = 0; i < base.getNumSamples(); i++) {
                target.addSample(channel, i + currentDelayPosition, base.getSample(channel, i));
            }
        }

        applyDelay(target, base, dampen, delayTimeInSamples, iteration + 1);
    }
}

void ProcessingThreadPoolJob::applyReverb(AudioSampleBuffer &target, const char *fileName, const size_t fileSize) {
    AudioSampleBuffer copy;
    copy.makeCopyOf(target);

    int processedSize = static_cast<int>(fileSize) + copy.getNumSamples() - 1;
    target.setSize(target.getNumChannels(), processedSize, false, true, AVOID_REALLOCATING);

    ProcessSpec processSpec = {sampleRate, static_cast<uint32>(processedSize),
                               static_cast<uint32>(target.getNumChannels())};

    convolution.prepare(processSpec);
    convolution.loadImpulseResponse(fileName, fileSize, false, true, 0);

    AudioBlock<float> audioBlockIn = AudioBlock<float>(copy);
    AudioBlock<float> audioBlockOut = AudioBlock<float>(target);
    ProcessContextNonReplacing<float> processContext = ProcessContextNonReplacing<float>(audioBlockIn, audioBlockOut);
    convolution.process(processContext);
}


ThreadPoolJob::JobStatus ProcessingThreadPoolJob::runJob() {
    AudioSampleBuffer delayBaseTempBuffer;
    float delayFeedbackNormalized = guiParams.delayFeedback / 100.0f;
    float delayTimeNormalized = guiParams.delayTime / 1000.0f;
    auto delayTimeInSamples = static_cast<int>(sampleRate * delayTimeNormalized);
    AudioSampleBuffer delayBaseBuffer;
    delayBaseBuffer.makeCopyOf(bufferIn);
    time_t start, end;

    int timeWarp = type == RISE ? guiParams.riseTimeWarp : guiParams.fallTimeWarp;
    if (timeWarp != 0) {
        time(&start);
        applyTimeWarp(bufferIn, timeWarp);
        time(&end);
        printf("%d time warp elapsed: %.2lf seconds\n", type, difftime(end, start));
    }

    time(&start);
    applyDelay(bufferIn, delayBaseBuffer, delayFeedbackNormalized, delayTimeInSamples, 1);
    time(&end);
    printf("%d delay elapsed: %.2lf seconds\n", type, difftime(end, start));

    time(&start);
    applyReverb(bufferIn, BinaryData::room_impulse_response_LBS_wav, BinaryData::room_impulse_response_LBS_wavSize);
    time(&end);
    printf("%d reverb elapsed: %.2lf seconds\n", type, difftime(end, start));

    if ((type == RISE && guiParams.riseReverse) || (type == FALL && guiParams.fallReverse)) {
        time(&start);
        bufferIn.reverse(0, bufferIn.getNumSamples());
        time(&end);
        printf("%d reverse elapsed: %.2lf seconds\n", type, difftime(end, start));
    }

    return jobHasFinished;
}

AudioSampleBuffer ProcessingThreadPoolJob::getOutputBuffer() {
    return bufferIn;
}


