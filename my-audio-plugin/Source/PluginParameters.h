#ifndef PLUGINPARAMETERS_H
#define PLUGINPARAMETERS_H

#include <JuceHeader.h>

class PluginParameters
{
public:
    PluginParameters();
    ~PluginParameters();

    void addParameter(const String& parameterID, const String& parameterName, float defaultValue);
    float getParameterValue(const String& parameterID) const;
    void setParameterValue(const String& parameterID, float newValue);

private:
    AudioProcessorValueTreeState parameters;
};

#endif // PLUGINPARAMETERS_H