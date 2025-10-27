#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class Analyzer
{
public:
    explicit Analyzer (int fftOrder)
    : order (fftOrder), fft (fftOrder),
      window (1 << fftOrder, juce::dsp::WindowingFunction<float>::hann, false)
    {
        fftSize = 1 << order;
        hop = fftSize / 4;
        fifo.resize (fftSize, 0.0f);
        magDb.assign (numBands, 0.0f);
    }
    
    void setAnalysisSpeed (int speedIndex)
    {
        // 0=Slow (0.95), 1=Medium (0.90), 2=Fast (0.80)
        if (speedIndex == 0) smoothingCoeff = 0.95f;
        else if (speedIndex == 1) smoothingCoeff = 0.90f;
        else smoothingCoeff = 0.80f;
    }

    void pushSamples (const float* x, int n)
    {
        // Very simple ring; this is sufficient for the starter.
        for (int i=0; i<n; ++i)
        {
            fifo[wrIdx] = x[i];
            wrIdx = (wrIdx + 1) % fftSize;
            if (++accum >= hop)
            {
                accum = 0;
                compute();
                readyFlag = true;
            }
        }
    }

    bool ready() { bool r = readyFlag; readyFlag = false; return r; }
    std::vector<float> getSmoothedMagDb() const { return magDb; }

private:
    void compute()
    {
        temp.assign (fftSize * 2, 0.0f);
        // Copy time data into real part, then apply window
        for (int i=0;i<fftSize;++i)
            temp[i] = fifo[(wrIdx + i) % fftSize];
        
        window.multiplyWithWindowingTable (temp.data(), fftSize);

        fft.performRealOnlyForwardTransform (temp.data());

        std::vector<float> mags (fftSize/2);
        for (int i=0;i<fftSize/2;++i)
        {
            float re = temp[i*2], im = temp[i*2+1];
            float m = std::sqrt (re*re + im*im) + 1.0e-9f;
            mags[i] = juce::Decibels::gainToDecibels (m);
        }

        // Downsample to 32 pseudo-log bands with smoothing
        for (int b=0;b<numBands;++b)
        {
            int a = (b   ) * (fftSize/2) / numBands;
            int z = (b+1 ) * (fftSize/2) / numBands;
            float s = 0.0f; int c=0;
            for (int i=a;i<z;++i){ s += mags[i]; ++c; }
            float newVal = s / juce::jmax(1,c);
            magDb[b] = smoothingCoeff * magDb[b] + (1.0f - smoothingCoeff) * newVal;
        }
    }

    int order=11, fftSize=2048, hop=512;
    juce::dsp::FFT fft;
    juce::dsp::WindowingFunction<float> window;
    std::vector<float> fifo, temp, magDb;
    int wrIdx=0, accum=0;
    bool readyFlag=false;
    float smoothingCoeff = 0.90f; // default Medium speed
    static constexpr int numBands = 32;
};
