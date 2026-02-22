#pragma once

#include <JuceHeader.h>
#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

class SteppedSlider : public juce::Slider
{
public:
    using juce::Slider::Slider;

    void setDrawMarkers (bool shouldDraw) noexcept
    {
        drawMarkers = shouldDraw;
    }

    bool getDrawMarkers() const noexcept
    {
        return drawMarkers;
    }

    void setAllowedValues (std::vector<double> values,
                           bool shouldSortAndUnique = true,
                           bool shouldUpdateRange = true)
    {
        if (values.empty())
        {
            allowedValues.clear();
            return;
        }

        if (shouldSortAndUnique)
        {
            std::sort (values.begin(), values.end());
            values.erase (std::unique (values.begin(), values.end()), values.end());
        }

        allowedValues = std::move (values);

        if (shouldUpdateRange)
            setRange (allowedValues.front(), allowedValues.back());
    }

    const std::vector<double>& getAllowedValues() const noexcept
    {
        return allowedValues;
    }

protected:
    double snapValue (double attemptedValue, DragMode dragMode) override
    {
        if (allowedValues.empty())
            return juce::Slider::snapValue (attemptedValue, dragMode);

        return findNearestValue (allowedValues, attemptedValue);
    }

    double proportionOfLengthToValue (double proportion) override
    {
        if (allowedValues.empty())
            return juce::Slider::proportionOfLengthToValue (proportion);

        const int lastIndex = static_cast<int> (allowedValues.size()) - 1;
        if (lastIndex <= 0)
            return allowedValues.front();

        const double clamped = juce::jlimit (0.0, 1.0, proportion);
        const int index = juce::jlimit (0, lastIndex,
                                        static_cast<int> (std::round (clamped * lastIndex)));
        return allowedValues[static_cast<size_t> (index)];
    }

    double valueToProportionOfLength (double value) override
    {
        if (allowedValues.empty())
            return juce::Slider::valueToProportionOfLength (value);

        const int lastIndex = static_cast<int> (allowedValues.size()) - 1;
        if (lastIndex <= 0)
            return 0.0;

        int closestIndex = 0;
        double bestDistance = std::abs (value - allowedValues.front());

        for (int i = 1; i < static_cast<int> (allowedValues.size()); ++i)
        {
            const double distance = std::abs (value - allowedValues[static_cast<size_t> (i)]);
            if (distance < bestDistance)
            {
                bestDistance = distance;
                closestIndex = i;
            }
        }

        return static_cast<double> (closestIndex) / static_cast<double> (lastIndex);
    }

private:
    static double findNearestValue (const std::vector<double>& values, double value)
    {
        double closest = values.front();
        double bestDistance = std::abs (value - closest);

        for (const auto candidate : values)
        {
            const double distance = std::abs (value - candidate);
            if (distance < bestDistance)
            {
                bestDistance = distance;
                closest = candidate;
            }
        }

        return closest;
    }

    std::vector<double> allowedValues;
    bool drawMarkers = true;
};

inline juce::NormalisableRange<float> makeSteppedNormalisableRange (std::vector<float> values)
{
    jassert (! values.empty());

    if (values.empty())
        return juce::NormalisableRange<float> (0.0f, 1.0f);

    std::sort (values.begin(), values.end());
    values.erase (std::unique (values.begin(), values.end()), values.end());

    const int lastIndex = static_cast<int> (values.size()) - 1;

    auto findNearestIndex = [values] (float value) -> int
    {
        int closestIndex = 0;
        float bestDistance = std::abs (value - values.front());

        for (int i = 1; i < static_cast<int> (values.size()); ++i)
        {
            const float distance = std::abs (value - values[static_cast<size_t> (i)]);
            if (distance < bestDistance)
            {
                bestDistance = distance;
                closestIndex = i;
            }
        }

        return closestIndex;
    };

    auto convertFrom0To1 = [values, lastIndex] (float, float, float proportion)
    {
        if (lastIndex <= 0)
            return values.front();

        const float clamped = juce::jlimit (0.0f, 1.0f, proportion);
        const int index = juce::jlimit (0, lastIndex, static_cast<int> (std::round (clamped * lastIndex)));
        return values[static_cast<size_t> (index)];
    };

    auto convertTo0To1 = [values, lastIndex, findNearestIndex] (float, float, float value)
    {
        if (lastIndex <= 0)
            return 0.0f;

        const int index = findNearestIndex (value);
        return static_cast<float> (index) / static_cast<float> (lastIndex);
    };

    auto snapToLegalValue = [values] (float, float, float value)
    {
        float closest = values.front();
        float bestDistance = std::abs (value - closest);

        for (const auto candidate : values)
        {
            const float distance = std::abs (value - candidate);
            if (distance < bestDistance)
            {
                bestDistance = distance;
                closest = candidate;
            }
        }

        return closest;
    };

    return juce::NormalisableRange<float> (values.front(), values.back(),
                                           convertFrom0To1, convertTo0To1, snapToLegalValue);
}
