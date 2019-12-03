#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>

#include <QThread>
#include <QObject>
#include <QDebug>
#include <QQuickWindow>

#include "control_midi.h"
#include "backend.h"
#include "defs.h"

#define BLUEPRINT_USB_DATA_PACKET_SIZE 64
#define BLUEPRINT_USB_HID_PACKET_SIZE (BLUEPRINT_USB_DATA_PACKET_SIZE + 1)

/*!
 * \brief BackEnd::BackEnd
 * \param parent
 */
BackEnd::BackEnd(QObject *parent) :
    QObject(parent)
{

    setDeviceStatus(Unplugged);       //  Select Variable. Values: "unplugged"; "connected"; "wrongData"; "okData"; "working"
    conf_state = Request;
    preset_status = Request_Preset;
    sync_status = Request_Sync;
    memset(layout, 0, sizeof (layout));
    packet_num_buffer = 0;
    setComponentChannel(255);
    setComponentMinValue(0);
    setComponentMaxValue(127);

    m_sync = false;

    memset(&configuration, 0, sizeof (configuration));
    memset(&preset_array, 0, sizeof (preset_array));

    this->timerLoop = new QTimer(this);
    connect(timerLoop, &QTimer::timeout, this, &BackEnd::timer_timeout);
    timerLoopInterval = 100;
    this->timerLoop->start(timerLoopInterval);

    this->timerRead = new QTimer(this);
    connect(timerRead, &QTimer::timeout, this, &BackEnd::timerRead_timeout);
    timerReadInterval = 100;

    this->timerSync = new QTimer(this);
    connect(timerSync, &QTimer::timeout, this, & BackEnd::timerSync_timeout);
    timerSyncInterval = 100;
}

BackEnd::~BackEnd()
{
    this->timerLoop->stop();

    if(m_deviceStatus != Unplugged){
        hid_close(md1_device);
        qDebug() << "Released Interface\n";
    }
}

/*!
 * TIMER OVERFLOW MAIN LOOP
 * \brief BackEnd::timer_timeout
 */
void BackEnd::timer_timeout(){
    switch (m_deviceStatus) {
    case Unplugged:
        senseDeviceStatus();
        break;
    case Ready_for_config:
        readDeviceConfiguration(conf_state);
        break;
    case Ready_for_update:
        break;
    case Ok_data:
        setLayout();
        break;
    case Component:
        setDeviceStatus(Working);
        setPreset(0);
        emit presetChanged();
        readPreset(0);
        break;
    case Working:
        senseValue();
        break;
    case Wrong_data:
        readDeviceConfiguration(conf_state);
        break;
    }
}

/*!
 * TIMER TO READ OVERFLOW
 * \brief BackEnd::timerRead_timeout
 */
void BackEnd::timerRead_timeout(){
    switch (preset_status) {
    case Request_Preset:
        redPresetReq(m_preset);
        break;
    case WaitFinish_Preset:
        WaitFinishPreset(m_preset);
        break;
    }
}

/*!
 * TIMER SYNC OVERFLOW
 * \brief BackEnd::timerSync_timeout
 */
void BackEnd::timerSync_timeout(){
    switch (sync_status) {
    case Request_Sync:
        syncReq();
        break;
    case SendLayout_Sync:
        SendLayoutSync();
        break;
    case SendFinish_Sync:
        SendFinishSync();
        break;
    case SendReqPreset_Sync:
        SendReqPresetSync();
        break;
    case WaitOK1_Sync:
        WaitOK1Sync();
        break;
    case SendPreset_Sync:
        SendPresetSync();
        break;
    case SendFinish1_Sync:
        SendFinish1Sync();
        break;
    }
}

/*!
 * \brief BackEnd::senseDeviceStatus
 */
void BackEnd::senseDeviceStatus(){

    this->timerLoop->stop();
    setDeviceStatus(Unplugged);

    pollUSB(0x04D8, 0x0053);
    if(deviceConnected){
        qWarning("Attempting to open device...");
        open(0x04D8, 0x0053);
        setDeviceStatus(Ready_for_config);
    }

    pollUSB(0x04D8, 0x003C);
    if(deviceConnected){
        qWarning("Attempting to open device to bootload...");
        open(0x04D8, 0x003C);
        setDeviceStatus(Ready_for_update);
    }

    this->timerLoop->start(timerLoopInterval);
    return;
}

void BackEnd::setDeviceStatus(const DeviceStatus &deviceStatus){

    if (deviceStatus == m_deviceStatus)
        return;

    m_deviceStatus = deviceStatus;
    emit deviceStatusChanged();
    return;
}

void BackEnd::pollUSB(uint16_t deviceVIDtoPoll, uint16_t devicePIDtoPoll)
{
    hid_device_info *dev;

    dev = hid_enumerate(deviceVIDtoPoll, devicePIDtoPoll);

    deviceConnected = (dev != nullptr);
    hid_free_enumeration(dev);
}

BackEnd::ErrorCode BackEnd::open(uint16_t deviceVIDtoOpen, uint16_t devicePIDtoOpen)
{
    md1_device = hid_open(deviceVIDtoOpen, devicePIDtoOpen, nullptr);
    if(md1_device){
        deviceConnected = true;
        hid_set_nonblocking(md1_device, true);
        qInfo("Device successfully connected to.");
        return Success;
    }

    qWarning("Unable to open device.");
    return NotConnected;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       DEVICE CONFIGURATION                                                          //
/// \brief BackEnd::senseDeviceStatus                                                                                   //
///
///  HOST REQUEST READING
//
//   ___________________________ ___________________________ ___________________________ ___________________________
//  |           SOF             |   HOST_REQUIRES_READING   |           CMD             |   HOST_REQUIRES_READING   |
//   --------------------------- --------------------------- --------------------------- ---------------------------
//
//  HOST REQUEST READING
//
//   ___________________________ ___________________________ ___________________________ ___________________________
//  |           SOF             |   HOST_REQUIRES_READING   |           CMD             |   HOST_REQUIRES_READING   |
//   --------------------------- --------------------------- --------------------------- ---------------------------
//
//  HOST REQUEST READING
//
//   ___________________________ ___________________________ ___________________________ ___________________________
//  |           SOF             |   HOST_REQUIRES_READING   |           CMD             |   HOST_REQUIRES_READING   |
//   --------------------------- --------------------------- --------------------------- ---------------------------                                                                                       //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BackEnd::readDeviceConfiguration(ConfigStatus &conf_state){

    unsigned char data_out[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to write
    unsigned char data_in[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to read

    qDebug() << conf_state;

    switch (conf_state) {
    case Request:
        memset(data_out,0,BLUEPRINT_USB_HID_PACKET_SIZE);
        qDebug() << "\nReading device configuration\n";
        this->timerLoop->stop();
        data_out[0]=0; data_out[1]=SOF; data_out[2]=HOST_REQUIRES_READING; data_out[3]=CMD_LAYOUT; data_out[4]=0;

        hid_write(md1_device,data_out,BLUEPRINT_USB_HID_PACKET_SIZE);
        qDebug() << "Read request send";
        if (hid_read_timeout(md1_device, data_in, BLUEPRINT_USB_HID_PACKET_SIZE, 1000) < 0){
            setDeviceStatus(Unplugged);
            conf_state = Request;
            qDebug() << "read error";
            this->timerLoop->start(timerLoopInterval);
            return;
        }
        conf_state = WaitFinish;
        packet_num_buffer = 0;
        this->timerLoop->start(100);
        break;

    case WaitFinish:
        this->timerLoop->stop();
        qDebug() << "Reading Data";
        int offset;

        memset(data_in,0,BLUEPRINT_USB_HID_PACKET_SIZE);
        if (hid_read_timeout(md1_device, data_in, BLUEPRINT_USB_HID_PACKET_SIZE, 1000) < 0){
            qDebug() << " write error";
            setDeviceStatus(Unplugged);
            conf_state = Request;
            this->timerLoop->start(timerLoopInterval);
            return;
        }

        if(data_in[1] == FINISH){
            setDeviceStatus(Ok_data);
            conf_state = Request;
            qDebug() << "Done";
            this->timerLoop->start(timerLoopInterval);
            break;
        }

        offset = packet_num_buffer * BLUEPRINT_USB_DATA_PACKET_SIZE;

        for (int i = 0; i < BLUEPRINT_USB_DATA_PACKET_SIZE; i++) {
            layout[offset + i] = data_in[i];
            qDebug() << "Layout R:" << offset + i << ":" << layout[offset + i];
        }

        packet_num_buffer = packet_num_buffer+1;
        this->timerLoop->start(timerLoopInterval);
        break;
    }
    return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                     SET LAYOUT
/// \brief BackEnd::setLayout
///                                     ENHANCE
///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BackEnd::setLayout(){

    this->timerLoop->stop();

    qDebug() << "Setting Layout";

    setControl0Type(layout[0]);
    setControl1Type(layout[1]);
    setControl2Type(layout[2]);
    setControl3Type(layout[3]);
    setControl4Type(layout[4]);
    setControl5Type(layout[5]);
    setControl6Type(layout[6]);
    setControl7Type(layout[7]);
    setControl8Type(layout[8]);
    setControl9Type(layout[9]);
    setControl10Type(layout[10]);
    setControl11Type(layout[11]);
    setControl12Type(layout[12]);
    setControl13Type(layout[13]);
    setControl14Type(layout[14]);
    setControl15Type(layout[15]);

    qDebug() << "Done\n";
    setDeviceStatus(Component);
    this->timerLoop->start(timerLoopInterval);
    return;
}

/*!
 * \brief BackEnd::setControl0Type
 * \param controlType
 */
void BackEnd::setControl0Type(const unsigned char &controlType){
    if (controlType == m_controlType[0])
        return;
    m_controlType[0] = controlType;
    emit control0TypeChanged();
    return;
}

void BackEnd::setControl1Type(const unsigned char &controlType){
    if (controlType == m_controlType[1])
        return;
    m_controlType[1] = controlType;
    emit control1TypeChanged();
    return;
}

void BackEnd::setControl2Type(const unsigned char &controlType){
    if (controlType == m_controlType[2])
        return;
    m_controlType[2] = controlType;
    emit control2TypeChanged();
    return;
}

void BackEnd::setControl3Type(const unsigned char &controlType){
    if (controlType == m_controlType[3])
        return;
    m_controlType[3] = controlType;
    emit control3TypeChanged();
    return;
}

void BackEnd::setControl4Type(const unsigned char &controlType){
    if (controlType == m_controlType[4])
        return;
    m_controlType[4] = controlType;
    emit control4TypeChanged();
    return;
}

void BackEnd::setControl5Type(const unsigned char &controlType){
    if (controlType == m_controlType[5])
        return;
    m_controlType[5] = controlType;
    emit control5TypeChanged();
    return;
}

void BackEnd::setControl6Type(const unsigned char &controlType){
    if (controlType == m_controlType[6])
        return;
    m_controlType[6] = controlType;
    emit control6TypeChanged();
    return;
}

void BackEnd::setControl7Type(const unsigned char &controlType){
    if (controlType == m_controlType[7])
        return;
    m_controlType[7] = controlType;
    emit control7TypeChanged();
    return;
}

void BackEnd::setControl8Type(const unsigned char &controlType){
    if (controlType == m_controlType[8])
        return;
    m_controlType[8] = controlType;
    emit control8TypeChanged();
    return;
}

void BackEnd::setControl9Type(const unsigned char &controlType){
    if (controlType == m_controlType[9])
        return;
    m_controlType[9] = controlType;
    emit control9TypeChanged();
    return;
}


void BackEnd::setControl10Type(const unsigned char &controlType){
    if (controlType == m_controlType[10])
        return;
    m_controlType[10] = controlType;
    emit control10TypeChanged();
    return;
}


void BackEnd::setControl11Type(const unsigned char &controlType){
    if (controlType == m_controlType[11])
        return;
    m_controlType[11] = controlType;
    emit control11TypeChanged();
    return;
}


void BackEnd::setControl12Type(const unsigned char &controlType){
    if (controlType == m_controlType[12])
        return;
    m_controlType[12] = controlType;
    emit control12TypeChanged();
    return;
}


void BackEnd::setControl13Type(const unsigned char &controlType){
    if (controlType == m_controlType[13])
        return;
    m_controlType[13] = controlType;
    emit control13TypeChanged();
    return;
}


void BackEnd::setControl14Type(const unsigned char &controlType){
    if (controlType == m_controlType[14])
        return;
    m_controlType[14] = controlType;
    emit control14TypeChanged();
    return;
}

void BackEnd::setControl15Type(const unsigned char &controlType){
    if (controlType == m_controlType[15])
        return;
    m_controlType[15] = controlType;
    emit control15TypeChanged();
    return;
}

void BackEnd::setSynchronizing(bool &sync){
    if (sync == m_sync)
        return;
    m_sync = sync;
    emit synchronizingChanged();
    return;
}

///////////////////////////////////////////////////////////////
///
///             Attributes
///
///////////////////////////////////////////////////////////////

/*!
 * \brief BackEnd::setPreset
 * \param preset
 */
void BackEnd::setPreset(const unsigned char &preset){
    if (preset == m_preset or m_deviceStatus != Working)
        return;
    m_preset = preset;
    m_globalChannel = layout[BLUEPRINT_PRESET_GLOBAL_CHANNEL_INDEX + preset];
    bool already_read = false;
    for (int i = 0; i < 16; ++i) {
        if (m_preset == preset_array[i])
            already_read = true;
    }
    if (!already_read){
        readPreset(m_preset);
        for (int i = 1; i < 16; ++i) {
            if (preset_array[i] == 0){
                preset_array[i] = m_preset;
                break;
            }
        }
    }
    selectComponent(0);
    emit presetChanged();
    emit globalChannelChanged();
    emit control0TypeChanged();
    emit control1TypeChanged();
    emit control2TypeChanged();
    emit control3TypeChanged();
    emit control4TypeChanged();
    emit control5TypeChanged();
    emit control6TypeChanged();
    emit control7TypeChanged();
    emit control8TypeChanged();
    emit control9TypeChanged();
    emit control10TypeChanged();
    emit control11TypeChanged();
    emit control12TypeChanged();
    emit control13TypeChanged();
    emit control14TypeChanged();
    emit control15TypeChanged();
}

void BackEnd::setComponentMode(const ComponentMode &deviceMode){
    int index = BLUEPRINT_PRESET_DATA_SIZE * m_preset + BLUEPRINT_CONTROL_DATA_SIZE * m_component;

    qDebug() << index << m_preset << m_component << deviceMode;

    if (deviceMode == m_componentMode or m_deviceStatus != Working)
        return;

    configuration.preset[m_preset].component[m_component].bytes.mode = static_cast<unsigned char>(deviceMode);

    m_componentMode = deviceMode;

    emit componentModeChanged();
    return;
}

void BackEnd::setComponentChannel(const unsigned char &controlChannel){

    if(controlChannel == m_componentChannel or m_deviceStatus != Working)
        return;
    if (controlChannel >= 16){
        configuration.preset[m_preset].component[m_component].bytes.channel = 0xFF;
        m_componentChannel = 0xFF;
    } else {
        configuration.preset[m_preset].component[m_component].bytes.channel = controlChannel;
        m_componentChannel = controlChannel;
    }

    emit componentChannelChanged();
    return;
}

void BackEnd::setGlobalChannel(const unsigned char &globalChannel){
    if(globalChannel == m_globalChannel or m_deviceStatus != Working)
        return;
    qDebug() << "Global channel:" << globalChannel << "Preset:" << m_preset;
    m_globalChannel = globalChannel;
    layout[BLUEPRINT_PRESET_GLOBAL_CHANNEL_INDEX + m_preset] = m_globalChannel;
    emit globalChannelChanged();
    return;
}

void BackEnd::setComponentData(const unsigned char &controlData){
    if(controlData == m_componentData or m_deviceStatus != Working)
        return;

    configuration.preset[m_preset].component[m_component].bytes.data = controlData;
    m_componentData = controlData;

    emit componentDataChanged();
    return;
}

void BackEnd::setComponentMinValue(const unsigned char &minValue){
    if (minValue == m_componentMinValue or m_deviceStatus != Working)
        return;

    configuration.preset[m_preset].component[m_component].bytes.min = minValue;
    m_componentMinValue = minValue;

    emit componentMinValueChanged();
    return;
}

void BackEnd::setComponentMaxValue(const unsigned char &maxValue){
    if (maxValue == m_componentMaxValue or m_deviceStatus != Working)
        return;

    configuration.preset[m_preset].component[m_component].bytes.max = maxValue;
    m_componentMaxValue = maxValue;

    emit componentMaxValueChanged();
    return;
}

void BackEnd::setComponentButtonBehaviour(const ComponentButtonBehaviour &controlButtonBehaviour){
    if (controlButtonBehaviour == m_componentButtonBehaviour or m_deviceStatus != Working)
        return;

    configuration.preset[m_preset].component[m_component].bytes.config = static_cast<uint8_t>(controlButtonBehaviour);

    m_componentButtonBehaviour = controlButtonBehaviour;
    emit componentButtonBehaviourChanged();
    return;
}

/*!
 * \brief BackEnd::selectComponent
 * \param component
 */
void BackEnd::selectComponent(const unsigned char &component){
    unsigned char config;
    unsigned char mode;
    unsigned char channel;
    unsigned char data;
    unsigned char minValue;
    unsigned char maxValue;

    m_component = component;

    qDebug()<< "Selecting component" << component;

    config = configuration.preset[m_preset].component[m_component].bytes.config;
    mode = configuration.preset[m_preset].component[m_component].bytes.mode;
    channel = configuration.preset[m_preset].component[m_component].bytes.channel;
    data = configuration.preset[m_preset].component[m_component].bytes.data;
    minValue = configuration.preset[m_preset].component[m_component].bytes.min;
    maxValue = configuration.preset[m_preset].component[m_component].bytes.max;

    setComponentButtonBehaviour(static_cast<ComponentButtonBehaviour>(config));
    setComponentMode(static_cast<ComponentMode>(mode));
    setComponentChannel(channel);
    setComponentData(data);
    setComponentMinValue(minValue);
    setComponentMaxValue(maxValue);


    emit componentChanged();

    return;
}

/*!
 * SELECT  & READ PRESET
 * \brief BackEnd::readPreset
 * \param preset
 */
void BackEnd::readPreset(const unsigned char &preset){

    this->timerLoop->stop();
    m_preset = preset;
    packet_num_buffer = 0;
    this ->timerRead->start(timerReadInterval);
    return;

}

void BackEnd::redPresetReq(const unsigned char &preset){

    this ->timerRead->stop();

    unsigned char data_out[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to write
    unsigned char data_in[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to read

    memset(data_out,0,BLUEPRINT_USB_HID_PACKET_SIZE);
    memset(data_in,0,BLUEPRINT_USB_HID_PACKET_SIZE);

    data_out[0]=0; data_out[1]=SOF; data_out[2]=HOST_REQUIRES_READING; data_out[3]=CMD_PRESET; data_out[4]=preset;

    hid_write(md1_device,data_out,BLUEPRINT_USB_HID_PACKET_SIZE);if (hid_read_timeout(md1_device, data_in, BLUEPRINT_USB_HID_PACKET_SIZE, 1000) < 0){
        setDeviceStatus(Unplugged);
        conf_state = Request;
        qDebug() << "read error";
        this->timerLoop->start(timerLoopInterval);
        return;
    }

    qDebug() << "Read Preset request send" << preset;

    preset_status = WaitFinish_Preset;//preset_status = WaitOK_Preset;
    this->timerRead->start(timerReadInterval);
    return;

}

void BackEnd::WaitFinishPreset(const unsigned char &preset){

    this -> timerRead->stop();

    unsigned char data_in[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to read

    memset(data_in,0,BLUEPRINT_USB_HID_PACKET_SIZE);

    qDebug() << "Ok\n\nReading Data\n";

    if (hid_read_timeout(md1_device, data_in, BLUEPRINT_USB_HID_PACKET_SIZE, 1000) < 0){
        setDeviceStatus(Unplugged);
        qDebug() << "read error";
        this->timerLoop->start(timerLoopInterval);
        return;
    }

    if(data_in[1] == FINISH){
        preset_status = Request_Preset;
        this->timerRead->stop();
        selectComponent(0);
        this->timerLoop->start(timerLoopInterval);
        return;
    }

    for (int i = 0; i < BLUEPRINT_USB_DATA_PACKET_SIZE; i++) {
        qDebug() << "Preset packet b:" << i << data_in[i];
        configuration.preset[preset].packet[packet_num_buffer].data[i] = data_in[i];
    }

    packet_num_buffer = packet_num_buffer + 1;
    this ->timerRead->start(timerReadInterval);
    return;
}

/*!
 * \brief BackEnd::syncHost2Device
 */
void BackEnd::syncHost2Device(){
    if(m_deviceStatus != Working)
        return;
    bool temp_variable = true;
    setSynchronizing(temp_variable);
    this->timerLoop->stop();
    packet_num_buffer = 0;
    this ->timerSync->start(timerSyncInterval);
    return;
}

void BackEnd::syncReq(){
    qDebug() << "Sync";

    unsigned char data_out[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to write
    unsigned char data_in[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to read

    memset(data_out,0,BLUEPRINT_USB_HID_PACKET_SIZE);
    memset(data_in,0,BLUEPRINT_USB_HID_PACKET_SIZE);

    data_out[0]=0; data_out[1]=SOF; data_out[2]=HOST_REQUIRES_WRITING; data_out[3]=CMD_LAYOUT; data_out[4]=0;

    hid_write(md1_device,data_out,BLUEPRINT_USB_HID_PACKET_SIZE);

    if (hid_read_timeout(md1_device, data_in, BLUEPRINT_USB_HID_PACKET_SIZE, 1000) < 0){
        qDebug() << " write error";
        setDeviceStatus(Unplugged);
        this->timerSync->stop();
        this->timerLoop->start(timerLoopInterval);
        return;
    }

    qDebug() << "Read request send";
    sync_status = SendLayout_Sync;
}

void BackEnd::SendLayoutSync(){

    qDebug() << "Writting Data (Layout) packet: " << packet_num_buffer;

    unsigned char data_out[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to write

    //memset(data_out,0,BLUEPRINT_USB_HID_PACKET_SIZE);

    int offset = BLUEPRINT_USB_DATA_PACKET_SIZE * packet_num_buffer;

    data_out[0] = 0;
    if(packet_num_buffer == 1)
        data_out[BLUEPRINT_USB_DATA_PACKET_SIZE] = 0xAA;
    for (unsigned char i = 0; i < BLUEPRINT_USB_DATA_PACKET_SIZE; i++){
        data_out[i + 1] = layout[offset + i];
        qDebug() << "Layout" << offset + i << ":" << layout[offset + i];
    }

    if (hid_write(md1_device,data_out,BLUEPRINT_USB_HID_PACKET_SIZE) < 0){
        qDebug() << "write error";
        setDeviceStatus(Unplugged);
        this->timerSync->stop();
        this->timerLoop->start(timerLoopInterval);
        return;
    }
    ++packet_num_buffer;
    QThread::msleep(100);
    if (packet_num_buffer > 1){
        packet_num_buffer = 0;
        sync_status = SendFinish_Sync;
    }
    return;
}

void BackEnd::SendFinishSync(){

    unsigned char data_out[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to write
    memset(data_out,0,BLUEPRINT_USB_HID_PACKET_SIZE);

    data_out[0]=0; data_out[1]=SOF; data_out[2]=FINISH; data_out[3]=CMD_LAYOUT; data_out[4]=FINISH;

    if (hid_write(md1_device,data_out,BLUEPRINT_USB_HID_PACKET_SIZE) < 0){
        qDebug() << " write error";
        setDeviceStatus(Unplugged);
        this->timerSync->stop();
        this->timerLoop->start(timerLoopInterval);
        return;
    }
    QThread::msleep(500);
    sync_status = SendReqPreset_Sync;
    return;
}

void BackEnd::SendReqPresetSync(){

    unsigned char data_out[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to write
    memset(data_out,0,BLUEPRINT_USB_HID_PACKET_SIZE);

    data_out[0]=0; data_out[1]=SOF; data_out[2]=HOST_REQUIRES_WRITING; data_out[3]=CMD_CHANGES; data_out[4]=m_preset;

    if (hid_write(md1_device,data_out,BLUEPRINT_USB_HID_PACKET_SIZE) < 0){
        qDebug() << " write error";
        setDeviceStatus(Unplugged);
        this->timerSync->stop();
        this->timerLoop->start(timerLoopInterval);
        return;
    }

    qDebug() << "Write Preset Request Send";

    QThread::msleep(500);
    sync_status = SendPreset_Sync;
    return;
}

void BackEnd::WaitOK1Sync(){

    unsigned char data_in[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to read
    memset(data_in,0,BLUEPRINT_USB_HID_PACKET_SIZE);

    if (hid_read_timeout(md1_device, data_in, BLUEPRINT_USB_HID_PACKET_SIZE, 1000) < 0){
        setDeviceStatus(Unplugged);
        qDebug() << " read error";
        this->timerSync->stop();
        this->timerLoop->start(timerLoopInterval);
        return;
    }

    if((data_in[0] == SOF) && (data_in[1] == OK) ){
        qDebug() << "Ok";
        sync_status = SendPreset_Sync;
    }
    return;
}

void BackEnd::SendPresetSync(){

    unsigned char data_out[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to write
    memset(data_out,0,BLUEPRINT_USB_HID_PACKET_SIZE);
    qDebug() << "Sending preset" << m_preset << " package" << packet_num_buffer;

    int offset = BLUEPRINT_PRESET_DATA_SIZE * m_preset + BLUEPRINT_USB_DATA_PACKET_SIZE * packet_num_buffer;

    for (int i = 0; i < 16; ++i) {
        if(configuration.preset[m_preset].component[i].bytes.channel == 255 || configuration.preset[m_preset].component[i].bytes.channel == 16)
            configuration.preset[m_preset].component[i].bytes.channel = m_globalChannel;
    }

    data_out[0] = 0;
    for(int i = 0; i < BLUEPRINT_USB_DATA_PACKET_SIZE; i++){
        data_out[i + 1] = configuration.preset[m_preset].packet[packet_num_buffer].data[i];
        qDebug() << offset + i << " === " << data_out[i + 1];
    }

    if(hid_write(md1_device,data_out,BLUEPRINT_USB_HID_PACKET_SIZE) < 0){
        setDeviceStatus(Unplugged);
        qDebug() << " read error";
        this->timerSync->stop();
        this->timerLoop->start(timerLoopInterval);
        return;
    }
    ++packet_num_buffer;
    QThread::msleep(100);
    if(packet_num_buffer >= 4){
        sync_status = SendFinish1_Sync;
        packet_num_buffer = 0;
    }
    return;
}

void BackEnd::SendFinish1Sync(){

    unsigned char data_out[BLUEPRINT_USB_HID_PACKET_SIZE]; //data to write
    memset(data_out,0,BLUEPRINT_USB_HID_PACKET_SIZE);

    data_out[0]=0; data_out[1]=SOF; data_out[2]=FINISH; data_out[3]=CMD_CHANGES; data_out[4]=m_preset;

    if (hid_write(md1_device,data_out,BLUEPRINT_USB_HID_PACKET_SIZE) < 0){
        qDebug() << " write error";
        setDeviceStatus(Unplugged);
        this->timerSync->stop();
        this->timerLoop->start(timerLoopInterval);
        return;
    }

    qDebug() << "Already sync!";
    QThread::msleep(500);
    setDeviceStatus(Working);

    sync_status = Request_Sync;
    bool temp_variable = false;
    setSynchronizing(temp_variable);
    this->timerSync->stop();
    this->timerLoop->start(timerLoopInterval);
    return;
}

/*!
 * \brief BackEnd::senseValue
 */
void BackEnd::senseValue(){
    //return;
    this->timerLoop->stop();

    unsigned char data_in[BLUEPRINT_USB_HID_PACKET_SIZE];

    // GGG, changed timeout from 1000 to 0
    if (hid_read_timeout(md1_device, data_in, BLUEPRINT_USB_HID_PACKET_SIZE, 0) < 0) {
        setDeviceStatus(Unplugged);
        qDebug() << "error\n";
        this->timerLoop->start(timerLoopInterval);
        return;
    }
    this->timerLoop->start(timerLoopInterval);
    return;
}

