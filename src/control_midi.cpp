#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>

#include "control_midi.h"
#include "defs.h"

#include <QObject>
#include <QDebug>
#include <QQuickWindow>


Control_midi::Control_midi(QObject *parent) : QObject(parent)
{
    set_minValue(0);
    set_maxValue(127);

}

Control_midi::~Control_midi(){}

void Control_midi::set_index(const unsigned char &index){
    if (index == m_index)
        return;
    m_index = index;
    emit indexChanged();
    return;
}

void Control_midi::set_preset(const unsigned char &preset){
    if (preset == m_preset)
        return;
    m_preset = preset;
    emit presetChanged();
    return;
}

void Control_midi::set_componentType(const unsigned char &componentType){

    /*if (componentType == m_componentType)
        return;*/
    m_componentType = componentType;

    emit componentTypeChanged();
    //qDebug() << "NO SE QUE ES";
    return;
}

void Control_midi::set_mode(const QString &mode){

    if (mode == m_mode)
        return;
/*
    if (deviceMode == "Voice Note")
        interfaceConfig[32 + 5*m_component] = voiceNote;
    if (deviceMode == "PP")
        interfaceConfig[32 + 5*m_component] = voicePolyPressure;
    if (deviceMode == "CC")
        interfaceConfig[32 + 5*m_component] = voiceControlChange;
    if (deviceMode == "PC")
        interfaceConfig[32 + 5*m_component] = voiceProgramChange;
    if (deviceMode == "CP")
        interfaceConfig[32 + 5*m_component] = voiceChannelPreassure;
    if (deviceMode == "PB")
        interfaceConfig[32 + 5*m_component] = voicePitchBend;
    */
    m_mode = mode;
    emit modeChanged();
    return;
}

void Control_midi::set_ccNumber(const unsigned char &ccNumber){
    if(ccNumber == m_ccNumber)
        return;
   // interfaceConfig[34 + 5*m_component] = ccNumber;
    m_ccNumber = ccNumber;
    emit ccNumberChanged();
    return;
}


void Control_midi::set_minValue(const unsigned char &minValue){

    if (minValue == m_minValue)
        return;
    //interfaceConfig[35 +5*m_component] = minValue;
    m_minValue = minValue;
    emit minValueChanged();
    return;
}

void Control_midi::set_maxValue(const unsigned char &maxValue){
    if (maxValue == m_maxValue)
        return;
    //interfaceConfig[36 +5*m_component] = maxValue;
    m_maxValue = maxValue;
    emit maxValueChanged();
    return;
}

