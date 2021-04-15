#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>

#include <QThread>
#include <QObject>
#include <QDebug>
#include <QQuickWindow>
#include <QByteArray>
#include <QVariant>
#include <QMessageBox>

#include "control_midi.h"
#include "backend.h"
#include "defs.h"

#define PRESET_PACKET_SIZE 64
#define LAYOUT_PACKET_SIZE 32
#define BLUEPRINT_USB_HID_PACKET_SIZE (PRESET_PACKET_SIZE + 1)

#define SYSEX_COMMAND_PUSH_LAYOUT   0x1
#define SYSEX_COMMAND_PULL_LAYOUT   0x2
#define SYSEX_COMMAND_PUSH_PRESET   0x3
#define SYSEX_COMMAND_PULL_PRESET   0x4
#define SYSEX_COMMAND_SYSTEM       0x5
#define SYSEX_COMMAND_BULK_XFER    0x6

#define MIDI_MFR_ID_0           0x00
#define MIDI_MFR_ID_1           0x7F
#define MIDI_MFR_ID_2           0x00

BackEnd *mBackend;

void mycallback(double deltatime, std::vector<unsigned char> *message, void *)
{
    unsigned int nBytes = message->size();
    uint8_t message_type = message->at(0) >> 4;
    uint8_t channel = message->at(0) & 0x0F;
    uint8_t data = message->at(1);
    uint8_t value = message->at(2);
    uint8_t sysex_cmd;
    uint8_t preset;
    uint8_t packet_number;

    //  Channels [1-16] -> 1 -> 0, 16 -> 15
    switch (message_type) {
    case 0x08:
        qDebug() << "Note Off:      " << "Channel:" << channel + 1 << ", Note:" << data << ", Velocity:" << value;
        break;
    case 0x09:
        qDebug() << "Note On:       " << "Channel:" << channel + 1 << ", Note:" << data << ", Velocity:" << value;
        break;
    case 0x0b:
        qDebug() << "Control Change:" << "Channel:" << channel + 1 << ", Data:" << data << ", Value:   " << value;
        break;
    case 0x0f:
        //  SysEx
        if((message->at(1) == MIDI_MFR_ID_0) && (message->at(2) == MIDI_MFR_ID_1) && (message->at(3) == MIDI_MFR_ID_2)){
            sysex_cmd = message->at(4);
            switch (sysex_cmd) {
            case SYSEX_COMMAND_PUSH_LAYOUT:
                qDebug() << "Push Layout Done";
                mBackend->sysExCmdDone = true;
                break;
            case SYSEX_COMMAND_PULL_LAYOUT: //  Pull layout
                qDebug() << "Pull Layout Done";
                for (unsigned int i = 0; i < nBytes - 6; i++) {
                    mBackend->layout[i] = message->at(i + 5);
                    //                    qDebug() << "Layout R:" << i << ":" << mBackend->layout[i];
                }

                mBackend->sysExCmdDone = true;
                break;
            case SYSEX_COMMAND_PUSH_PRESET:
                qDebug() << "Push Preset Done";
                mBackend->sysExCmdDone = true;
                break;
            case SYSEX_COMMAND_PULL_PRESET:
                preset = message->at(5);
                packet_number = message->at(6);

                qDebug() << QString("Pull Preset Packet %1 done").arg(packet_number);

                for (int i = 0; i < PRESET_PACKET_SIZE; i++)
                {
                    mBackend->configuration.preset[preset].packet[packet_number].data[i] = message->at(i + 7);
                    //                    qDebug() << "Preset packet b:" << i << message->at(i + 7);
                }

                if (packet_number == 3)
                    mBackend->sysExCmdDone = true;

                break;
            }
        }
        break;
    }
}

// This function should be embedded in a try/catch block in case of
// an exception.  It offers the user a choice of MIDI ports to open.
// It returns false if there are no ports available.
bool chooseMidiPort( RtMidi *rtmidi )
{
    unsigned int port = 0;
    bool isInput = false;
    if ( typeid( *rtmidi ) == typeid( RtMidiIn ) )
        isInput = true;

    QString portName;
    unsigned int i = 0, nPorts = rtmidi->getPortCount();
    if ( nPorts == 0 ) {
        if ( isInput )
            qDebug() << "No input ports available!";
        else
            qDebug() << "No output ports available!";
        return false;
    }

    for ( i = 0; i < nPorts; i++ ) {
        portName = QString::fromStdString(rtmidi->getPortName(i));
        if ( isInput )
            qDebug() << "  Input port #" << i << ": " << portName << '\n';
        else
            qDebug() << "  Output port #" << i << ": " << portName << '\n';

        if (portName.contains("MD1 MIDI Controller")){
            if ( isInput )
                qDebug() << "Input MD1 MIDI Controller found! port #" << i;
            else
                qDebug() << "Output MD1 MIDI Controller found! port #" << i;
            QThread::msleep( 100 ); // pause a little
            rtmidi->openPort( i );
            return true;
        }
    }

    qDebug() << "No MD1 MIDI Controller found!";

    return false;
}

/*!
 * CONSTRUCTOR
 * \brief BackEnd::BackEnd
 * \param parent
 */
BackEnd::BackEnd(QObject *parent) :
    QObject(parent)
{
    mBackend = this;

    setDeviceStatus(Unplugged);       //  Select Variable. Values: "unplugged"; "connected"; "wrongData"; "okData"; "working"
    layout_status = PullLayoutReq;
    preset_status = PullPresetReq;
    push_layout_status = PushLayoutReq;
    push_preset_status = PushPresetReq;
    sync_status = SendLayout_Sync;
    memset(layout, 0, sizeof (layout));
    packet_num_buffer = 0;
    setComponentChannel(255);
    setComponentMinValue(0);
    setComponentMaxValue(127);

    m_sync = false;

    memset(&configuration, 0, sizeof (configuration));
    memset(&preset_array, 0, sizeof (preset_array));

    this->timerLoop = new QTimer(this);
    connect(timerLoop, &QTimer::timeout, this, &BackEnd::main_state_machine);
    timerLoopInterval = 100;
    this->timerLoop->start(timerLoopInterval);

    // RtMidiOut and RtMidiIn constructors
    try {
        midiout = new RtMidiOut();
        midiin = new RtMidiIn();
    }
    catch ( RtMidiError &error ) {
        error.printMessage();
    }
}

BackEnd::~BackEnd()
{
    this->timerLoop->stop();

    if(m_deviceStatus != Unplugged){
        delete midiin;
        delete midiout;
        //        hid_close(md1_device);
        qDebug() << "Released Interface\n";
    }
}

void BackEnd::sysExSendMessage(uint8_t command, uint8_t length, uint8_t *buffer)
{
    std::vector<unsigned char> message;

    message.clear();
    message.push_back( 0xF0 );

    message.push_back( MIDI_MFR_ID_0 );
    message.push_back( MIDI_MFR_ID_1 );
    message.push_back( MIDI_MFR_ID_2 );
    message.push_back( command );

    for (unsigned char i = 0; i < length; i++)
    {
        message.push_back(buffer[i]);
    }

    message.push_back( 0xF7 );
    midiout->sendMessage( &message );

    sysExCmdDone = false;
}

void BackEnd::sysExSendMessage(uint8_t command)
{
    std::vector<unsigned char> message;

    message.clear();
    message.push_back( 0xF0 );

    message.push_back( MIDI_MFR_ID_0 );
    message.push_back( MIDI_MFR_ID_1 );
    message.push_back( MIDI_MFR_ID_2 );
    message.push_back( command );

    message.push_back( 0xF7 );
    midiout->sendMessage( &message );

    sysExCmdDone = false;
}

/*!
 * TIMER OVERFLOW MAIN LOOP
 * \brief BackEnd::timer_timeout
 */
void BackEnd::main_state_machine()
{
    switch (m_deviceStatus)
    {
    case BackEnd::Unplugged:
        searchDevice();
        break;
    case BackEnd::Push_Layout:
        PushLayout();
        break;
    case BackEnd::Pull_Layout:
        PullLayout();
        break;
    case BackEnd::Push_Preset:
        PushPreset(m_preset);
        break;
    case BackEnd::Pull_Preset:
        PullPreset(m_preset);
        break;
    case BackEnd::Idle:
        break;
    case BackEnd::Ready_for_update:
        break;
    case BackEnd::Ok_data:
        setLayout();
        break;
    case BackEnd::Component:
        setPreset(0);
        readPreset(0);
        break;
    }
}

/*!
 * DEVICE STATUS
 * \brief BackEnd::senseDeviceStatus
 */
void BackEnd::searchDevice()
{
    this->timerLoop->stop();
    setDeviceStatus(Unplugged);

    try {
        if (chooseMidiPort( midiin ) && chooseMidiPort( midiout )){
            qWarning("Attempting to open device...");
            setDeviceStatus(Pull_Layout);
        }
    }
    catch ( RtMidiError &error ) {
        error.printMessage();
        emit openError();
        return;
    }

    // Don't ignore sysex, timing, or active sensing messages.
    midiin->ignoreTypes( false, false, false );
    midiin->setCallback( &mycallback );

    this->timerLoop->start(timerLoopInterval);
    return;
}

void BackEnd::setDeviceStatus(const DeviceStatus &deviceStatus)
{
    if (deviceStatus == m_deviceStatus)
        return;

    m_deviceStatus = deviceStatus;
    emit deviceStatusChanged();
    return;
}

void BackEnd::PushLayout()
{
    std::vector<unsigned char> message;
    static int timeout;

    this->timerLoop->stop();

    switch (push_layout_status) {
    case PushLayoutReq:
        qDebug() << "Writting Data (Layout) packet: " << packet_num_buffer;
        sysExSendMessage(SYSEX_COMMAND_PUSH_LAYOUT, LAYOUT_PACKET_SIZE , layout );
        timeout = 5;
        push_layout_status = PushLayoutCheck;
        break;
    case PushLayoutCheck:
        if (timeout-- == 0){
            qDebug() << "Timeout";
            push_layout_status = PushLayoutReq;
            break;
        }

        if (sysExCmdDone){
            qDebug() << "Done";
            push_layout_status = PushLayoutReq;
            setDeviceStatus(Push_Preset);
            break;
        }
        break;
    }

    this ->timerLoop->start(timerLoopInterval);

    return;
}

void BackEnd::PullLayout()
{
    std::vector<unsigned char> message;
    static int timeout = 0;

    qDebug() << layout_status;

    this->timerLoop->stop();

    switch (layout_status) {
    case PullLayoutReq:
        qDebug() << "Reading device configuration";
        sysExSendMessage( SYSEX_COMMAND_PULL_LAYOUT );
        timeout = 5;
        layout_status = PullLayoutCheck;
        break;

    case PullLayoutCheck:
        if (timeout-- == 0){
            qDebug() << "timeout";
            setDeviceStatus(Unplugged);
            layout_status = PullLayoutReq;
            break;
        }

        if (sysExCmdDone){
            setDeviceStatus(Ok_data);
            layout_status = PullLayoutReq;
            qDebug() << "Done";
            break;
        }
        break;
    }

    this->timerLoop->start(timerLoopInterval);
    return;
}

void BackEnd::PullPreset(const unsigned char &preset)
{
    std::vector<unsigned char> message;
    static int timeout;

    qDebug() << preset_status;

    this->timerLoop->stop();

    switch (preset_status)
    {
    case PullPresetReq:
        qDebug() << "Read Preset request send" << preset;
        sysExSendMessage( SYSEX_COMMAND_PULL_PRESET, 1, (uint8_t *)&preset);
        timeout = 5;
        preset_status = PullPresetCheck;
        break;
    case PullPresetCheck:
        if (timeout-- == 0){
            qDebug() << "Timeout";
            preset_status = PullPresetReq;
            break;
        }

        if (sysExCmdDone){
            qDebug() << "Done";
            preset_status = PullPresetReq;
            setDeviceStatus(Idle);
            emit presetChanged();
            selectComponent(0);
            break;
        }

        break;
    }

    this->timerLoop->start(timerLoopInterval);
}

void BackEnd::PushPreset(const unsigned char &preset)
{
    std::vector<unsigned char> message;
    uint8_t buffer[66];
    static int timeout;

    qDebug() << push_preset_status;

    this->timerLoop->stop();

    switch (push_preset_status)
    {
    case PushPresetReq:
        qDebug() << "Read Preset request send" << preset;

        buffer[0] = preset;
        buffer[1] = packet_num_buffer;
        for (int i = 0; i < PRESET_PACKET_SIZE; i++ ){
            buffer[i+2] = mBackend->configuration.preset[preset].packet[packet_num_buffer].data[i];
        }

        sysExSendMessage(SYSEX_COMMAND_PUSH_PRESET, PRESET_PACKET_SIZE + 2, buffer);
        timeout = 5;
        packet_num_buffer++;

        push_preset_status = PushPresetCheck;
        break;
    case PushPresetCheck:
        if (timeout-- == 0){
            qDebug() << "Timeout";
            packet_num_buffer = 0;
            push_preset_status = PushPresetReq;
            break;
        }

        if (sysExCmdDone){
            qDebug() << "Done";
            push_preset_status = PushPresetReq;

            if (packet_num_buffer == 3){
                bool temp_variable = false;
                setSynchronizing(temp_variable);
                setDeviceStatus(Idle);
            }
            break;
        }

        break;
    }

    this->timerLoop->start(timerLoopInterval);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                               SET LAYOUT                                                            //
/// \brief BackEnd::setLayout                                                                                           //
///                                                                                                                     //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BackEnd::setLayout()
{
    this->timerLoop->stop();

    qDebug() << "Setting Layout";

    for(int i = 0;i<=15;i++)
    {
        m_controlType[i] = layout[i];
    }

    emit controlTypeChanged();

    m_preset = 255;
    setDeviceStatus(Component);

    this->timerLoop->start(timerLoopInterval);
    return;
}

/*!
 * \brief BackEnd::setControl0Type
 * \param controlType
 */
void BackEnd::SetControlType(QList<QVariant> &controlType)
{
    for(int i = 0; i<=m_controlType.size();i++){
        m_controlType[i] = controlType[i];
    }
    return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                                     //
///                                             SET ATRIBUTTES                                                          //
///                                                                                                                     //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief BackEnd::setPreset
 * \param preset
 */
void BackEnd::setPreset(const unsigned char &preset)
{
    if (preset == m_preset)
        return;
    m_preset = preset;
    m_globalChannel = layout[BLUEPRINT_PRESET_GLOBAL_CHANNEL_INDEX + preset];
    bool already_read = false;
    for (int i = 0; i < 16; ++i)
    {
        if (m_preset == preset_array[i])
            already_read = true;
    }
    if (!already_read)
    {
        readPreset(m_preset);
        for (int i = 1; i < 16; ++i)
        {
            if (preset_array[i] == 0)
            {
                preset_array[i] = m_preset;
                break;
            }
        }
    }
    selectComponent(0);
    emit presetChanged();
    emit globalChannelChanged();
    emit controlTypeChanged();
}

/*!
 * \brief BackEnd::setGlobalChannel
 * \param globalChannel
 */

void BackEnd::setGlobalChannel(const unsigned char &globalChannel)
{
    if(globalChannel == m_globalChannel)
        return;
    qDebug() << "Global channel:" << globalChannel << "Preset:" << m_preset;
    m_globalChannel = globalChannel;
    layout[BLUEPRINT_PRESET_GLOBAL_CHANNEL_INDEX + m_preset] = m_globalChannel;
    emit globalChannelChanged();
    return;
}

/*!
 * \brief BackEnd::setComponentMode
 * \param deviceMode
 */

void BackEnd::setComponentMode(const ComponentMode &deviceMode)
{
    if (deviceMode == m_componentMode)
        return;

    configuration.preset[m_preset].component[m_component].bytes.mode = static_cast<unsigned char>(deviceMode);
    m_componentMode = deviceMode;

    emit componentModeChanged();
    return;
}

/*!
 * CC NUMBER
 * \brief BackEnd::setComponentChannel
 * \param controlChannel
 */

void BackEnd::setComponentData(const unsigned char &controlData)
{
    if(controlData == m_componentData)
        return;

    configuration.preset[m_preset].component[m_component].bytes.data = controlData;
    m_componentData = controlData;

    emit componentDataChanged();
    return;
}

/*!
 * \brief BackEnd::setComponentChannel
 * \param controlChannel
 */

void BackEnd::setComponentChannel(const unsigned char &controlChannel)
{

    if(controlChannel == m_componentChannel)
        return;

    if (controlChannel >= 16)
    {
        configuration.preset[m_preset].component[m_component].bytes.channel = 0x7F;
        m_componentChannel = 0x7F;

    } else {
        configuration.preset[m_preset].component[m_component].bytes.channel = controlChannel;
        m_componentChannel = controlChannel;
    }

    emit componentChannelChanged();
    return;
}

/*!
 * \brief BackEnd::setComponentMinValue
 * \param minValue
 */

void BackEnd::setComponentMinValue(const unsigned char &minValue){
    if (minValue == m_componentMinValue)
        return;

    configuration.preset[m_preset].component[m_component].bytes.min = minValue;
    m_componentMinValue = minValue;

    emit componentMinValueChanged();
    return;
}

/*!
 * \brief BackEnd::setComponentMaxValue
 * \param maxValue
 */

void BackEnd::setComponentMaxValue(const unsigned char &maxValue)
{
    if (maxValue == m_componentMaxValue)
        return;

    configuration.preset[m_preset].component[m_component].bytes.max = maxValue;
    m_componentMaxValue = maxValue;

    emit componentMaxValueChanged();
    return;
}

/*!
 * \brief BackEnd::setComponentButtonBehaviour
 * \param controlButtonBehaviour
 */

void BackEnd::setComponentButtonBehaviour(const ComponentButtonBehaviour &controlButtonBehaviour)
{
    if (controlButtonBehaviour == m_componentButtonBehaviour)
        return;

    configuration.preset[m_preset].component[m_component].bytes.config = static_cast<uint8_t>(controlButtonBehaviour);

    m_componentButtonBehaviour = controlButtonBehaviour;
    emit componentButtonBehaviourChanged();
    return;
}

void BackEnd::setSynchronizing(bool &sync)
{
    if (sync == m_sync)
        return;
    m_sync = sync;
    emit synchronizingChanged();
    return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                                     //
///                                             SELECT COMPONENT                                                        //
///                                                                                                                     //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*!
 * \brief BackEnd::selectComponent
 * \param component
 */

void BackEnd::selectComponent(const unsigned char &component)
{
    /*if(component == m_component)
        return;*/
    m_component = component;
    unsigned char mode = configuration.preset[m_preset].component[m_component].bytes.mode;
    unsigned char data = configuration.preset[m_preset].component[m_component].bytes.data;
    unsigned char channel = configuration.preset[m_preset].component[m_component].bytes.channel;
    unsigned char minValue = configuration.preset[m_preset].component[m_component].bytes.min;
    unsigned char maxValue = configuration.preset[m_preset].component[m_component].bytes.max;
    unsigned char config = configuration.preset[m_preset].component[m_component].bytes.config; /* Button Behaviour*/

    qDebug()<< "Selecting component" << component << "mode " <<mode;

    setComponentMode(static_cast<ComponentMode>(mode));
    setComponentData(data);
    setComponentChannel(channel);
    setComponentMinValue(minValue);
    setComponentMaxValue(maxValue);
    setComponentButtonBehaviour(static_cast<ComponentButtonBehaviour>(config));

    emit componentChanged();

    return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                                     //
///                                             SELECT PRESET                                                           //
///                                                                                                                     //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief BackEnd::readPreset
 * \param preset
 */
void BackEnd::readPreset(const unsigned char &preset)
{
    this->timerLoop->stop();
    m_preset = preset;
    packet_num_buffer = 0;
    setDeviceStatus(Pull_Preset);
    this->timerLoop->start(timerLoopInterval);
    return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                                     //
///                                       HOST-DVICE SYNCRONIZATION                                                     //
///                                                                                                                     //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*!
 * \brief BackEnd::syncHost2Device
 */
void BackEnd::syncHost2Device()
{
    if(m_deviceStatus != Idle)
        return;

    bool temp_variable = true;
    setSynchronizing(temp_variable);
    packet_num_buffer = 0;
    setDeviceStatus(Push_Layout);
    return;
}
