#pragma once
#include <JuceHeader.h>

struct DesignedBand
{
    double f0 = 1000.0;
    double Q  = 1.0;
    float  gainDb = 0.0f;
    juce::dsp::IIR::Filter<float> biq[2];
};

class EQDesigner
{
public:
    void prepare (double sampleRate, int numCh)
    {
        sr = sampleRate; channels = numCh;
        bands.resize (8);
        const double centers[8] = { 40,100,250,500,1000,2500,5000,12000 };
        const double Qs[8]      = { 0.7,0.9,1.1,1.2,1.2,1.1,0.9,0.7 };
        for (int i=0;i<8;++i){ bands[i].f0=centers[i]; bands[i].Q=Qs[i]; }
        loadProfile();
        targetDb.assign (32, 0.0f);
    }

    std::vector<float> computeDiff (const std::vector<float>& cur, const std::vector<float>& tgt)
    {
        const int n = (int) juce::jmin (cur.size(), tgt.size());
        std::vector<float> diff (n);
        for (int i=0;i<n;++i) diff[i] = juce::jlimit (-6.0f, 6.0f, tgt[i] - cur[i]);
        return diff;
    }

    void designFromDiff (const std::vector<float>& diffDb, float spice)
    {
        // Map the 32-bin diff into 8 center bands by averaging nearby bins.
        auto avgAt = [&](double f0){
            // crude mapping by index: 0..31 -> 20Hz..20kHz (rough)
            // this is placeholder logic; refine as needed.
            int idx = (int) juce::jlimit (0, 31, (int) std::round ( (std::log2(f0/20.0) / std::log2(20000.0/20.0)) * 31.0) );
            return diffDb[ (size_t) idx ];
        };

        for (auto& b : bands)
        {
            float g = avgAt (b.f0) * juce::jlimit (0.0f, 1.0f, spice);
            b.gainDb = juce::jlimit (-6.0f, 6.0f, g);
            auto coeff = juce::dsp::IIR::Coefficients<float>::makePeakFilter ((double) sr, b.f0, b.Q,
                juce::Decibels::decibelsToGain (b.gainDb));
            for (int ch=0; ch<juce::jmin(2, channels); ++ch)
                *b.biq[ch].state = *coeff;
        }
    }

    const std::vector<DesignedBand>& getBands() const { return bands; }
    std::vector<float> getTargetCurveDb() const { return targetDb; }

    void loadProfile()
    {
        // Load a simple target from JSON at compile-time path macro HOTSAUCE_PROFILES_JSON
        juce::File f (juce::String (HOTSAUCE_PROFILES_JSON));
        if (! f.existsAsFile()) return;
        juce::var json = juce::JSON::parse (f);
        if (! json.isObject()) return;
        auto* obj = json.getDynamicObject();
        if (!obj) return;

        auto profile = obj->getProperty ("Modern R&B"); // default
        if (! profile.isObject()) return;

        auto* pobj = profile.getDynamicObject();
        auto curve = pobj->getProperty ("curve");
        if (! curve.isArray()) return;
        auto* arr = curve.getArray();
        targetDb.assign (32, 0.0f);
        // map freq,dB pairs crudely into 32 bins
        for (int i=0;i<32;++i)
        {
            // pick nearest pair
            float x = (float) i / 31.0f;
            float freq = 20.0f * std::pow (20000.0f / 20.0f, x);
            float best = 0.0f; float bestDist = 1.0e9f;
            for (auto& v : *arr)
            {
                auto* pt = v.getDynamicObject();
                if (!pt) continue;
            }
            // simpler: just convert the array to pairs and do nearest
            if (arr->size() > 0)
            {
                double bestDb = 0.0, bestD = 1.0e9;
                for (auto& val : *arr)
                {
                    if (!val.isArray()) continue;
                    auto* a = val.getArray();
                    if (a->size() != 2) continue;
                    double f0 = (double) a->getUnchecked(0);
                    double dB = (double) a->getUnchecked(1);
                    double d  = std::abs (std::log (freq) - std::log (f0));
                    if (d < bestD) { bestD = d; bestDb = dB; }
                }
                targetDb[i] = (float) bestDb;
            }
        }
    }

private:
    double sr = 44100.0;
    int channels = 2;
    std::vector<DesignedBand> bands;
    std::vector<float> targetDb;
};
