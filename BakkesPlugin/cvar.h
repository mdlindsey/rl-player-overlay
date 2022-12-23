#pragma once
#include <iostream>
#include "global.h"
#include "logging.h"

using namespace std;

class Cvar {
    public:
    inline static map<string, Cvar *> Map;
    inline static enum class DataType { Str, Int, Bool, Float };
    static Cvar * Get(string cvarName) { return Cvar::Map[cvarName]; }

    string name;
    DataType type;
    int min;
    int max;
    Cvar(string name, string defaultValue)
    {
        this->name = name;
        this->type = DataType::Str;
        Cvar::Map[name] = this;
        Global::CvarManager->registerCvar(name, defaultValue);
    }
    Cvar(string name, int defaultValue)
    {
        this->name = name;
        this->type = DataType::Int;
        Cvar::Map[name] = this;
        Global::CvarManager->registerCvar(name, to_string(defaultValue));
    }
    Cvar(string name, int defaultValue, int minValue, int maxValue)
    {
        this->name = name;
        this->min = minValue;
        this->max = maxValue;
        this->type = DataType::Int;
        Cvar::Map[name] = this;
        Global::CvarManager->registerCvar(name, to_string(defaultValue), "", true, true, minValue, true, maxValue);
    }

    
    template <typename Callback>
    Cvar(string name, string defaultValue, Callback onChange)
    {
        this->name = name;
        this->type = DataType::Str;
        Cvar::Map[name] = this;
        Global::CvarManager->registerCvar(name, defaultValue)
            .addOnValueChanged([this, onChange, name](string oldValue, CVarWrapper cvar) {
                string curValue = cvar.getStringValue();
                if (curValue != oldValue) onChange(name, curValue, oldValue);
            });
    }

    template <typename Callback>
    Cvar(string name, int defaultValue, Callback onChange)
    {
        this->name = name;
        this->type = DataType::Int;
        Cvar::Map[name] = this;
        string defaultValueStr = to_string(defaultValue);
        Global::CvarManager->registerCvar(name, defaultValueStr)
            .addOnValueChanged([this, onChange, name](string oldValueStr, CVarWrapper cvar) {
                int oldValue = stoi(oldValueStr);
                int curValue = cvar.getIntValue();
                if (curValue != oldValue) onChange(name, curValue, oldValue);
            });
    }

    template <typename Callback>
    Cvar(string name, int defaultValue, int minValue, int maxValue, Callback onChange)
    {
        this->name = name;
        this->min = minValue;
        this->max = maxValue;
        this->type = DataType::Int;
        Cvar::Map[name] = this;
        Global::CvarManager->registerCvar(name, to_string(defaultValue), "", true, true, minValue, true, maxValue)
            .addOnValueChanged([this, onChange, name](string oldValueStr, CVarWrapper cvar) {
                int oldValue = stoi(oldValueStr);
                int curValue = cvar.getIntValue();
                if (curValue != oldValue) onChange(name, curValue, oldValue);
            });
    }

    void RenderSmallInput(string label="", int width=32, bool labelLeft=false)
    {
        auto cvar = Global::CvarManager->getCvar(this->name);
        string cvarVal = cvar.getStringValue();
        if (labelLeft)
        {
			ImGui::Text(label.c_str());
			ImGui::SameLine();
        }
        ImGui::PushItemWidth(width);
        ImGui::InputText(!labelLeft ? label.c_str() : string("##" + this->name).c_str(), &cvarVal);
        cvar.setValue(cvarVal);
    }

    void RenderLargeInput(string label="", int width=256)
    {
        auto cvar = Global::CvarManager->getCvar(this->name);
        string cvarVal = cvar.getStringValue();
        ImGui::Text(label.c_str());
        ImGui::PushItemWidth(width);
        ImGui::InputText(string("##" + this->name).c_str(), &cvarVal);
        cvar.setValue(cvarVal);
    }

    void RenderSliderInput(string label="", int width=256, bool labelNewLine = false)
    {
        if (this->type != DataType::Int && this->type != DataType::Float)
        {
            return;
        }
        auto cvar = Global::CvarManager->getCvar(this->name);
        int cvarVal = cvar.getIntValue();
        if (labelNewLine)
        {
            ImGui::Text(label.c_str());
        }
        ImGui::PushItemWidth(width);
        int min = this->min != NULL ? this->min : 0;
        int max = this->max != NULL ? this->max : 256;
        string sliderLabel = !labelNewLine ? label : "##" + this->name;
		ImGui::SliderInt(sliderLabel.c_str(), &cvarVal, min, max);
        cvar.setValue(cvarVal);
    }
};
